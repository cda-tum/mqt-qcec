import unittest

from jkq import qcec
from qiskit import QuantumCircuit


class QCECConstructionTests(unittest.TestCase):
    def setUp(self):
        # create a very simple quantum circuit
        self.qc1 = QuantumCircuit(1)
        self.qc1.h(0)
        # create a second (identical) circuit
        self.qc2 = QuantumCircuit(1)
        self.qc2.h(0)

        self.file1 = "../circuits/original/mlp4_245.real"
        self.file2 = "../circuits/transpiled/mlp4_245_transpiled.qasm"

        self.config = qcec.Configuration()

    def test_default_constructor_with_qiskit(self):
        """Test constructing an instance from two qiskit circuits with all default arguments"""
        qcec.EquivalenceCheckingManager(circ1=self.qc1, circ2=self.qc2)

    def test_default_constructor_with_file(self):
        """Test constructing an instance from two circuit files with all default arguments"""
        qcec.EquivalenceCheckingManager(circ1=self.file1, circ2=self.file2)

    def test_constructor_with_configuration(self):
        """Test constructing an instance from circuits and a configuration object"""
        qcec.EquivalenceCheckingManager(circ1=self.qc1, circ2=self.qc2, config=self.config)

    def test_constructor_with_kwargs(self):
        """Test constructing an instance from circuits and a (random) keyword argument"""
        qcec.EquivalenceCheckingManager(circ1=self.qc1, circ2=self.qc2, timeout=60.)
