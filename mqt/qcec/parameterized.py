"""Functionality for checking equivalence of parameterized quantum circuits."""

from __future__ import annotations

from itertools import chain
from typing import Any

import numpy as np
from mqt.qcec import Configuration, EquivalenceCheckingManager
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter, ParameterExpression


def __ecm_from_config_or_kwargs(
    circ1: QuantumCircuit, circ2: QuantumCircuit, configuration: Configuration | None = None, **kwargs: Any
) -> EquivalenceCheckingManager:
    if kwargs:
        # create the equivalence checker from keyword arguments
        return EquivalenceCheckingManager(circ1, circ2, **kwargs)
    else:
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


def extract_params(circ1: QuantumCircuit, circ2: QuantumCircuit) -> tuple[list[Parameter], np.array, np.array]:
    """Extract parameters and equations of parameterized circuits."""
    p1 = set(circ1.parameters)
    p2 = set(circ2.parameters)

    p = p1.union(p2)

    n_params = len(p)
    exprs = list(chain(*[instr[0].params for instr in circ1.data + circ2.data if instr[0].params != []]))

    def is_expr(x: float | Parameter | ParameterExpression) -> bool:
        return isinstance(x, (Parameter, ParameterExpression))

    symb_params = [param for param in p if is_expr(param)]
    symb_params.sort(key=lambda param: param.name)
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
) -> EquivalenceCheckingManager.EquivalenceCheckingManager.Results:
    """Equivalence checking flow for parameterized circuit."""
    res = check_parameterized_zx(circ1, circ2, configuration, **kwargs)

    if res.considered_equivalent():
        return res

    n_checks, tol = __parse_args(configuration, **kwargs)

    parameters, A, offsets = extract_params(circ1, circ2)

    def instantiate_params(
        qc1: QuantumCircuit, qc2: QuantumCircuit, b: np.array
    ) -> tuple[QuantumCircuit, QuantumCircuit]:
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

        return (qc1_bound, qc2_bound)

    def instantiate_params_zero(qc1: QuantumCircuit, qc2: QuantumCircuit) -> tuple[QuantumCircuit, QuantumCircuit]:
        return instantiate_params(qc1, qc2, offsets)

    def instantiate_params_phases(qc1: QuantumCircuit, qc2: QuantumCircuit) -> tuple[QuantumCircuit, QuantumCircuit]:
        phases = [0, np.pi, np.pi / 2, -np.pi / 2, np.pi / 4, -np.pi / 4]
        b = np.random.choice(phases, size=len(offsets)) + offsets
        return instantiate_params(qc1, qc2, b)

    if n_checks > 0:
        circ1_inst, circ2_inst = instantiate_params_zero(circ1, circ2)
        res = check_instantiated(circ1_inst, circ2_inst, configuration, **kwargs)
        if res.equivalence == "not_equivalent":
            return res

    for _i in range(1, n_checks - 1):
        circ1_inst, circ2_inst = instantiate_params_phases(circ1, circ2)
        res = check_instantiated(circ1_inst, circ2_inst, configuration, **kwargs)
        if res.equivalence == "not_equivalent":
            return res

    res = check_instantiated_random(circ1, circ2, parameters, configuration, **kwargs)

    return res
