"""Functionality for checking equivalence of parameterized quantum circuits."""

from __future__ import annotations

from itertools import chain
from typing import Any

import numpy as np
from mqt.qcec import EquivalenceCheckingManager
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter, ParameterExpression


def check_symbolic_zx(
    circ1: QuantumCircuit, circ2: QuantumCircuit, **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """Check circuits for equivalence with the zx_calculus."""
    ecm = EquivalenceCheckingManager(circ1, circ2, **kwargs)
    ecm.set_zx_checker(True)
    ecm.set_alternating_checker(False)
    ecm.set_simulation_checker(False)
    ecm.set_parallel(False)

    ecm.run()

    return ecm.get_results()


def extract_params(circ1: QuantumCircuit, circ2: QuantumCircuit) -> set[Parameter]:
    """Extract parameters and equations of symbolic parameterized circuits."""
    p1 = set(circ1.parameters)
    p2 = set(circ2.parameters)

    p = p1.union(p2)

    n_params = len(p)
    exprs = list(chain(*[instr[0].params for instr in circ1.data + circ2.data if instr[0].params != []]))

    def is_expr(x):
        return type(x) is Parameter or type(x) is ParameterExpression

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
    circ1: QuantumCircuit, circ2: QuantumCircuit, **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """Check circuits for equivalence with dd equivalence checker."""
    ecm = EquivalenceCheckingManager(circ1, circ2, **kwargs)
    ecm.set_zx_checker(False)
    ecm.set_alternating_checker(True)
    ecm.set_simulation_checker(True)

    ecm.run()

    return ecm.get_results()


def check_instantiated_random(
    circ1: QuantumCircuit, circ2: QuantumCircuit, params: set(Parameter), **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """Check whether circuits are equivalent for random instantiation of symbolic parameters."""
    param_map = {}
    for p in params:
        param_map[p] = np.random.rand() * 2 * np.pi

    circ1_inst = circ1.bind_parameters(param_map)
    circ2_inst = circ2.bind_parameters(param_map)
    ecm = EquivalenceCheckingManager(circ1_inst, circ2_inst, **kwargs)
    ecm.set_zx_checker(False)
    ecm.set_alternating_checker(True)
    ecm.set_simulation_checker(True)

    ecm.run()

    return ecm.get_results()


def check_symbolic(
    circ1: QuantumCircuit, circ2: QuantumCircuit, n_checks: int = 6, tol: float = 1e-15, **kwargs: Any
) -> EquivalenceCheckingManager.EquivalenceCheckingManager.Results:
    """Equivalence checking flow for parameterized circuit."""
    res = check_symbolic_zx(circ1, circ2, **kwargs)

    if res.considered_equivalent():
        return res

    parameters, A, offsets = extract_params(circ1, circ2)

    def instantiate_params(i, qc1: QuantumCircuit, qc2: QuantumCircuit) -> tuple[QuantumCircuit, QuantumCircuit]:
        phases = [0, np.pi, np.pi / 2, -np.pi / 2, np.pi / 4, -np.pi / 4]
        if i > 0:
            b = np.random.choice(phases, size=len(offsets)) + offsets
        else:
            b = offsets

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

    for i in range(0, n_checks - 1):

        circ1_inst, circ2_inst = instantiate_params(i, circ1, circ2, **kwargs)
        res = check_instantiated(circ1_inst, circ2_inst)
        if res.equivalence == "not_equivalent":
            return res

    res = check_instantiated_random(circ1, circ2, parameters, **kwargs)

    return res
