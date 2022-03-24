import pytest

from mqt import qcec
from qiskit import QuantumCircuit


@pytest.fixture
def example_circuit():
    qc = QuantumCircuit(1)
    qc.h(0)
    return qc


def test_default_constructor_with_qiskit(example_circuit):
    """Test constructing an instance from two qiskit circuits with all default arguments"""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit)


def test_constructor_with_configuration(example_circuit):
    """Test constructing an instance from circuits and a configuration object"""
    config = qcec.Configuration()
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, config=config)


def test_constructor_with_kwargs(example_circuit):
    """Test constructing an instance from circuits and a (random) keyword argument"""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, timeout=60.)


def test_default_constructor_with_file():
    """Test constructing an instance from two circuit files with all default arguments"""
    file1 = "../circuits/original/mlp4_245.real"
    file2 = "../circuits/transpiled/mlp4_245_transpiled.qasm"
    qcec.EquivalenceCheckingManager(circ1=file1, circ2=file2)
