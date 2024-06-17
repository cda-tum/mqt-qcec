"""Functionality for checking equivalence of parameterized quantum circuits."""

from __future__ import annotations

import time
from itertools import chain
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from numpy.typing import NDArray
    from qiskit import QuantumCircuit

import numpy as np
from qiskit.circuit import Parameter, ParameterExpression

from . import Configuration, EquivalenceCheckingManager, EquivalenceCriterion


def __is_parameterized(qc: QuantumCircuit | str) -> bool:
    return not isinstance(qc, str) and qc.parameters


def __adjust_timeout(curr_timeout: float, res: EquivalenceCheckingManager.Results | float) -> float:
    if curr_timeout == 0:
        return 0

    if isinstance(res, EquivalenceCheckingManager.Results):
        return curr_timeout - (res.check_time + res.preprocessing_time)
    return curr_timeout - res


def check_parameterized_zx(
    circ1: QuantumCircuit | str, circ2: QuantumCircuit | str, configuration: Configuration
) -> EquivalenceCheckingManager.Results:
    """Check circuits for equivalence with the ZX-calculus."""
    ecm = EquivalenceCheckingManager(circ1, circ2, configuration)
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
    symb_params.sort(key=lambda param: param.name)
    symb_exprs = list(filter(is_expr, exprs))

    offsets = np.zeros(len(symb_exprs))
    for row, expr in enumerate(symb_exprs):
        zero_map = dict.fromkeys(expr.parameters, 0)
        offsets[row] = -float(expr.bind(zero_map))

    equs = np.zeros((len(symb_exprs), n_params))

    for col, param in enumerate(symb_params):
        for row, expr in enumerate(symb_exprs):
            one_map = dict.fromkeys(expr.parameters, 0)
            if param in expr.parameters:
                one_map[param] = 1
                val = float((expr + offsets[row]).bind(one_map))
                equs[row, col] = val

    return symb_params, equs, offsets


def check_instantiated(
    circ1: QuantumCircuit, circ2: QuantumCircuit, configuration: Configuration
) -> EquivalenceCheckingManager.Results:
    """Check circuits for equivalence with DD equivalence checker."""
    ecm = EquivalenceCheckingManager(circ1, circ2, configuration)
    ecm.set_zx_checker(False)

    ecm.run()

    return ecm.get_results()


def check_instantiated_random(
    circ1: QuantumCircuit, circ2: QuantumCircuit, params: list[Parameter], configuration: Configuration
) -> EquivalenceCheckingManager.Results:
    """Check whether circuits are equivalent for random instantiation of symbolic parameters."""
    param_map = {}
    rng = np.random.default_rng()
    for p in params:
        param_map[p] = rng.random() * 2 * np.pi

    circ1_inst = circ1.assign_parameters(param_map)
    circ2_inst = circ2.assign_parameters(param_map)

    return check_instantiated(circ1_inst, circ2_inst, configuration)


def check_parameterized(
    circ1: QuantumCircuit | str, circ2: QuantumCircuit | str, configuration: Configuration
) -> EquivalenceCheckingManager.Results:
    """Equivalence checking flow for parameterized circuit."""
    total_preprocessing_time = 0.0
    total_runtime = 0.0
    total_simulations_started = 0
    total_simulations_finished = 0

    def update_stats(res: EquivalenceCheckingManager.Results) -> None:
        nonlocal total_preprocessing_time, total_runtime, total_simulations_started, total_simulations_finished
        total_preprocessing_time += res.preprocessing_time
        total_runtime += res.check_time
        total_simulations_started += res.started_simulations
        total_simulations_finished += res.performed_simulations

    def write_stats(i: int, res: EquivalenceCheckingManager.Results) -> None:
        nonlocal total_preprocessing_time, total_runtime, total_simulations_started, total_simulations_finished
        res.check_time = total_runtime
        res.preprocessing_time = total_preprocessing_time
        res.started_simulations = total_simulations_started
        res.performed_simulations = total_simulations_finished
        res.performed_instantiations = i

    res = check_parameterized_zx(circ1, circ2, configuration)

    if res.considered_equivalent():
        return res

    update_stats(res)

    timeout = __adjust_timeout(configuration.execution.timeout, res)
    n_checks = configuration.parameterized.additional_instantiations
    tol = configuration.parameterized.parameterized_tolerance

    parameters, mat, offsets = extract_params(circ1, circ2)

    def instantiate_params(
        qc1: QuantumCircuit, qc2: QuantumCircuit, b: NDArray[np.float64]
    ) -> tuple[QuantumCircuit, QuantumCircuit, float]:
        start_time = time.time()
        mat_pinv = np.linalg.pinv(mat)
        x = np.dot(mat_pinv, b)
        param_map = {param: x[i] for i, param in enumerate(parameters)}
        qc1_bound = qc1.assign_parameters(param_map)
        qc2_bound = qc2.assign_parameters(param_map)

        def round_zero_params(qc: QuantumCircuit) -> QuantumCircuit:
            for instr in qc.data:
                if not hasattr(instr[0], "mutable") or instr[0].mutable:
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
        rng = np.random.default_rng()
        b = rng.choice(phases, size=len(offsets)) + offsets
        return instantiate_params(qc1, qc2, b)

    circ1_inst, circ2_inst, runtime = instantiate_params_zero(circ1, circ2)
    timeout = __adjust_timeout(timeout, runtime)

    if timeout < 0:
        write_stats(1, res)
        res.equivalence = EquivalenceCriterion.no_information
        return res

    res = check_instantiated(circ1_inst, circ2_inst, configuration)
    update_stats(res)
    if res.equivalence == EquivalenceCriterion.not_equivalent:
        write_stats(1, res)
        return res

    for i in range(n_checks):
        circ1_inst, circ2_inst, runtime = instantiate_params_phases(circ1, circ2)
        timeout = __adjust_timeout(timeout, runtime)
        res = check_instantiated(circ1_inst, circ2_inst, configuration)
        timeout = __adjust_timeout(timeout, res)

        if timeout < 0:
            write_stats(i + 2, res)
            res.equivalence = EquivalenceCriterion.no_information
            return res

        update_stats(res)

        if res.equivalence == EquivalenceCriterion.not_equivalent:
            write_stats(i + 2, res)
            return res

    res = check_instantiated_random(circ1, circ2, parameters, configuration)
    timeout = __adjust_timeout(timeout, runtime)
    if timeout < 0:
        res.equivalence = EquivalenceCriterion.no_information

    update_stats(res)
    write_stats(n_checks + 2, res)
    return res
