import pytest

from mqt import qcec
from qiskit import QuantumCircuit


@pytest.fixture
def example_circuit() -> QuantumCircuit:
    qc = QuantumCircuit(1)
    qc.h(0)
    return qc


def test_default_constructor_with_qiskit(example_circuit: QuantumCircuit):
    """Test constructing an instance from two qiskit circuits with all default arguments"""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit)


def test_constructor_with_configuration(example_circuit: QuantumCircuit):
    """Test constructing an instance from circuits and a configuration object"""
    config = qcec.Configuration()
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, config=config)


def test_constructor_with_kwargs(example_circuit: QuantumCircuit):
    """Test constructing an instance from circuits and a (random) keyword argument"""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, timeout=60.)


def test_default_constructor_with_file(example_circuit: QuantumCircuit):
    """Test constructing an instance from two circuit files with all default arguments"""
    import os
    filename = "test.qasm"
    example_circuit.qasm(filename=filename)
    qcec.EquivalenceCheckingManager(circ1=filename, circ2=filename)
    os.remove(filename)
