from qiskit import QuantumCircuit, transpile
from qiskit.test.mock import FakeAthens
from mqt import qcec

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

    # initialize the equivalence checker
    ecm = qcec.EquivalenceCheckingManager(circ, circ_comp)
    qcec.setup_compilation_flow_verification(ecm, optimization_level=optimization_level)

    # execute the check
    ecm.run()

    # obtain the result
    print(ecm.equivalence())
