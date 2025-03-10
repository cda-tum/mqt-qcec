"""A simple example of using QCEC."""

from __future__ import annotations

from qiskit import QuantumCircuit, transpile
from qiskit.providers.fake_provider import GenericBackendV2

from mqt.qcec import verify_compilation

if __name__ == "__main__":
    circ = QuantumCircuit(3)
    circ.x(2)
    circ.h(range(3))
    circ.ccx(0, 1, 2)
    circ.h(range(2))
    circ.x(range(2))
    circ.h(1)
    circ.cx(0, 1)
    circ.h(1)
    circ.x(range(2))
    circ.h(range(2))
    circ.measure_all()
    print(circ.draw(fold=-1))

    # define the target architecture
    backend = GenericBackendV2(
        num_qubits=5, coupling_map=[[0, 1], [1, 0], [1, 2], [2, 1], [1, 3], [3, 1], [3, 4], [4, 3]]
    )

    # compile the circuit to the target architecture
    optimization_level = 1
    circ_comp = transpile(circ, backend=backend, optimization_level=optimization_level, seed_transpiler=12345)
    print(circ_comp.draw(fold=-1))

    # verify the compilation
    result = verify_compilation(circ, circ_comp, optimization_level=optimization_level)

    # obtain the result
    print(result.equivalence)
