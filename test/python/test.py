"""A simple example of using QCEC."""

from __future__ import annotations

from mqt import qcec
from qiskit import QuantumCircuit, transpile
from qiskit.providers.fake_provider import FakeAthens

if __name__ == "__main__":
    # create quantum circuit for GHZ state
    circ = QuantumCircuit(3)
    circ.h(0)
    circ.cx(0, 1)
    circ.cx(0, 2)
    circ.measure_all()
    print(circ.draw(fold=-1))

    # compile the circuit to the 5-qubit IBMQ Athens architecture
    optimization_level = 1
    circ_comp = transpile(circ, backend=FakeAthens(), optimization_level=optimization_level)
    print(circ_comp.draw(fold=-1))

    # verify the compilation
    result = qcec.verify_compilation(circ, circ_comp, optimization_level=optimization_level)

    # obtain the result
    print(result.equivalence)
