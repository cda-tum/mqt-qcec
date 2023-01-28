from __future__ import annotations

import pytest
from qiskit import QuantumCircuit

from mqt import qcec


@pytest.fixture()
def example_circuit() -> QuantumCircuit:
    qc = QuantumCircuit(1)
    qc.h(0)
    return qc


def test_default_constructor_with_qiskit(example_circuit: QuantumCircuit) -> None:
    """Test constructing an instance from two qiskit circuits with all default arguments"""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit)


def test_constructor_with_configuration(example_circuit: QuantumCircuit) -> None:
    """Test constructing an instance from circuits and a configuration object"""
    config = qcec.Configuration()
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, config=config)


def test_constructor_with_kwargs(example_circuit: QuantumCircuit) -> None:
    """Test constructing an instance from circuits and a (random) keyword argument"""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, timeout=60.0)


def test_default_constructor_with_file(example_circuit: QuantumCircuit) -> None:
    """Test constructing an instance from two circuit files with all default arguments"""
    from pathlib import Path

    filename = "test.qasm"
    example_circuit.qasm(filename=filename)
    qcec.EquivalenceCheckingManager(circ1=filename, circ2=filename)
    Path(filename).unlink()
