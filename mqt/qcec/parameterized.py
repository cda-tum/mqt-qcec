"""Functionality for checking equivalence of parameterized quantum circuits."""

from __future__ import annotations

import time
from itertools import chain
from typing import Any

import numpy as np
from mqt.qcec import Configuration, EquivalenceCheckingManager, EquivalenceCriterion
from numpy.typing import NDArray
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter, ParameterExpression


def __is_parameterized(qc: QuantumCircuit) -> bool:
    return not isinstance(qc, str) and qc.parameters


def __adjust_timeout(curr_timeout: float | None, res: EquivalenceCheckingManager.Results | float) -> float | None:
    if curr_timeout is not None:
        if isinstance(res, EquivalenceCheckingManager.Results):
            return curr_timeout - (res.check_time + res.preprocessing_time)
        return curr_timeout - res
    return None


def __ecm_from_config_or_kwargs(
    circ1: QuantumCircuit, circ2: QuantumCircuit, configuration: Configuration | None = None, **kwargs: Any
) -> EquivalenceCheckingManager:
    if kwargs:
        # create the equivalence checker from keyword arguments
        return EquivalenceCheckingManager(circ1, circ2, **kwargs)
    if configuration is None:
        configuration = Configuration()

    # create the equivalence checker from configuration
    return EquivalenceCheckingManager(circ1, circ2, configuration)


def check_parameterized_zx(
    circ1: QuantumCircuit, circ2: QuantumCircuit, configuration: Configuration | None = None, **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """Check circuits for equivalence with the ZX-calculus."""
    ecm = __ecm_from_config_or_kwargs(circ1, circ2, configuration, **kwargs)
    ecm.disable_all_checkers()
    ecm.set_zx_checker(True)

    ecm.run()

    return ecm.get_results()


def extract_params(
    circ1: QuantumCircuit, circ2: QuantumCircuit
) -> tuple[list[Parameter], NDArray[np.float64], NDArray[np.float64]]:
    """Extract parameters and equations of parameterized circuits."""
    p1 = set(circ1.parameters)
    p2 = set(circ2.parameters)

    p = p1.union(p2)

    n_params = len(p)
    exprs = list(chain(*[instr[0].params for instr in circ1.data + circ2.data if instr[0].params != []]))

    def is_expr(x: float | Parameter | ParameterExpression) -> bool:
        return isinstance(x, (Parameter, ParameterExpression))

    symb_params: list[Parameter | ParameterExpression] = [param for param in p if is_expr(param)]
    symb_params.sort(key=lambda param: param.name)  # type: ignore[no-any-return]
    symb_exprs = list(filter(is_expr, exprs))

    offsets = np.zeros(len(symb_exprs))
    for row, expr in enumerate(symb_exprs):
        zero_map = {param: 0 for param in expr.parameters}
        offsets[row] = -float(expr.bind(zero_map))

    equs = np.zeros((len(symb_exprs), n_params))

    for col, param in enumerate(symb_params):
        for row, expr in enumerate(symb_exprs):
            one_map = {p: 0 for p in expr.parameters}
            if param in expr.parameters:
                one_map[param] = 1
                val = float((expr + offsets[row]).bind(one_map))
                equs[row, col] = val

    return symb_params, equs, offsets


def check_instantiated(
    circ1: QuantumCircuit, circ2: QuantumCircuit, configuration: Configuration | None = None, **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """Check circuits for equivalence with DD equivalence checker."""
    ecm = __ecm_from_config_or_kwargs(circ1, circ2, configuration, **kwargs)
    ecm.set_zx_checker(False)

    ecm.run()

    return ecm.get_results()


def check_instantiated_random(
    circ1: QuantumCircuit,
    circ2: QuantumCircuit,
    params: list[Parameter],
    configuration: Configuration | None = None,
    **kwargs: Any,
) -> EquivalenceCheckingManager.Results:
    """Check whether circuits are equivalent for random instantiation of symbolic parameters."""
    param_map = {}
    for p in params:
        param_map[p] = np.random.rand() * 2 * np.pi

    circ1_inst = circ1.bind_parameters(param_map)
    circ2_inst = circ2.bind_parameters(param_map)

    return check_instantiated(circ1_inst, circ2_inst, configuration, **kwargs)


def __parse_args(configuration: Configuration | None = None, **kwargs: Any) -> tuple[int, float]:
    n_checks = 0
    tol = 1e-12
    if kwargs:
        if kwargs.get("additional_instantiations"):
            n_checks = kwargs["additional_instantiations"]
        if kwargs.get("parameterized_tolerance"):
            tol = kwargs["parameterized_tolerance"]
    elif configuration is not None:
        n_checks = configuration.parameterized.additional_instantiations
        tol = configuration.parameterized.parameterized_tolerance
    return n_checks, tol


def check_parameterized(
    circ1: QuantumCircuit, circ2: QuantumCircuit, configuration: Configuration | None = None, **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """Equivalence checking flow for parameterized circuit."""
    total_preprocessing_time = 0.0
    total_runtime = 0.0
    total_simulations_started = 0
    total_simulations_finished = 0

    def update_stats(res: EquivalenceCheckingManager.Results) -> None:
        nonlocal total_preprocessing_time
        nonlocal total_runtime
        nonlocal total_simulations_started
        nonlocal total_simulations_finished
        total_preprocessing_time += res.preprocessing_time
        total_runtime += res.check_time
        total_simulations_started += res.started_simulations
        total_simulations_finished += res.performed_simulations

    def write_stats(i: int, res: EquivalenceCheckingManager.Results) -> None:
        nonlocal total_preprocessing_time
        nonlocal total_runtime
        nonlocal total_simulations_started
        nonlocal total_simulations_finished
        res.check_time = total_runtime
        res.preprocessing_time = total_preprocessing_time
        res.started_simulations = total_simulations_started
        res.performed_simulations = total_simulations_finished
        res.performed_instantiations = i

    res = check_parameterized_zx(circ1, circ2, configuration, **kwargs)

    if res.considered_equivalent():
        return res

    update_stats(res)

    if kwargs.get("timeout"):
        timeout = kwargs["timeout"].total_seconds()
    else:
        timeout = None

    timeout = __adjust_timeout(timeout, res)

    n_checks, tol = __parse_args(configuration, **kwargs)

    parameters, A, offsets = extract_params(circ1, circ2)

    def instantiate_params(
        qc1: QuantumCircuit, qc2: QuantumCircuit, b: NDArray[np.float64]
    ) -> tuple[QuantumCircuit, QuantumCircuit, float]:
        start_time = time.time()
        A_pinv = np.linalg.pinv(A)
        x = np.dot(A_pinv, b)
        param_map = {param: x[i] for i, param in enumerate(parameters)}
        qc1_bound = qc1.bind_parameters(param_map)
        qc2_bound = qc2.bind_parameters(param_map)

        def round_zero_params(qc: QuantumCircuit) -> QuantumCircuit:
            for instr in qc.data:
                params = instr[0].params
                instr[0].params = [float(x) for x in params]
                instr[0].params = [0 if np.abs(x) < tol else x for x in instr[0].params]
            return qc

        qc1_bound = round_zero_params(qc1_bound)
        qc2_bound = round_zero_params(qc2_bound)
        return qc1_bound, qc2_bound, time.time() - start_time

    def instantiate_params_zero(
        qc1: QuantumCircuit, qc2: QuantumCircuit
    ) -> tuple[QuantumCircuit, QuantumCircuit, float]:
        return instantiate_params(qc1, qc2, offsets)

    def instantiate_params_phases(
        qc1: QuantumCircuit, qc2: QuantumCircuit
    ) -> tuple[QuantumCircuit, QuantumCircuit, float]:
        phases = [0, np.pi, np.pi / 2, -np.pi / 2, np.pi / 4, -np.pi / 4]
        b = np.random.choice(phases, size=len(offsets)) + offsets
        return instantiate_params(qc1, qc2, b)

    circ1_inst, circ2_inst, runtime = instantiate_params_zero(circ1, circ2)
    timeout = __adjust_timeout(timeout, runtime)

    if timeout is not None and timeout < 0:
        write_stats(1, res)
        res.equivalence = EquivalenceCriterion.no_information
        return res

    res = check_instantiated(circ1_inst, circ2_inst, configuration, **kwargs)
    update_stats(res)
    if res.equivalence == EquivalenceCriterion.not_equivalent:
        write_stats(1, res)
        return res

    for i in range(n_checks):
        circ1_inst, circ2_inst, runtime = instantiate_params_phases(circ1, circ2)
        timeout = __adjust_timeout(timeout, runtime)
        res = check_instantiated(circ1_inst, circ2_inst, configuration, **kwargs)
        timeout = __adjust_timeout(timeout, res)

        if timeout is not None and timeout < 0:
            write_stats(i + 2, res)
            res.equivalence = EquivalenceCriterion.no_information
            return res

        update_stats(res)

        if res.equivalence == EquivalenceCriterion.not_equivalent:
            write_stats(i + 2, res)
            return res

    res = check_instantiated_random(circ1, circ2, parameters, configuration, **kwargs)
    timeout = __adjust_timeout(timeout, runtime)
    if timeout is not None and timeout < 0:
        res.equivalence = EquivalenceCriterion.no_information

    update_stats(res)
    write_stats(n_checks + 2, res)
    return res
