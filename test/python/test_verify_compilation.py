from __future__ import annotations

import pytest
from mqt import qcec
from qiskit import QuantumCircuit, transpile
from qiskit.providers.fake_provider import FakeAthens


@pytest.fixture
def original_circuit() -> QuantumCircuit:
    qc = QuantumCircuit(3)
    qc.h(0)
    qc.cx(0, 1)
    qc.cx(0, 2)
    qc.measure_all()
    return qc


@pytest.mark.parametrize("optimization_level", [0, 1, 2, 3])
def test_verify_compilation_on_optimization_levels(original_circuit: QuantumCircuit, optimization_level: int) -> None:
    """
    Test the verification of the compilation of a circuit
    to the 5-qubit IBMQ Athens architecture with various optimization levels.
    """
    compiled_circuit = transpile(original_circuit, backend=FakeAthens(), optimization_level=optimization_level)
    result = qcec.verify_compilation(original_circuit, compiled_circuit, optimization_level=optimization_level)
    assert (
        result.equivalence == qcec.EquivalenceCriterion.equivalent
        or result.equivalence == qcec.EquivalenceCriterion.equivalent_up_to_global_phase
    )
