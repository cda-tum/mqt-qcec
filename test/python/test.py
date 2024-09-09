"""A simple example of using QCEC."""

from __future__ import annotations

from mqt import qcec

if __name__ == "__main__":
    # for num_qubits in range(2, 32):
    #     circ = TwoLocal(num_qubits=num_qubits,
    #                     reps=num_qubits,
    #                     rotation_blocks=["rz"],
    #                     entanglement_blocks=["cx"],
    #                     entanglement="linear",
    #                     flatten=True)
    #     circ = circ.assign_parameters(np.random.rand(circ.num_parameters))
    #     # print(circ.draw(output="text"))
    #
    #     optimization_level = 1
    #     circ_comp = transpile(circ, basis_gates=["id", "x", "sx", "rz", "cz"], optimization_level=optimization_level)
    #     # print(circ_comp.draw(output="text"))
    #
    #     # verify the compilation
    #     start = timer()
    #     result = qcec.verify(circ, circ_comp, run_zx_checker=False, run_simulation_checker=False, run_alternating_checker=True, run_construction_checker=False)
    #     end = timer()
    #
    #     print(f"num_qubits: {num_qubits}\t runtime: {end - start}\t max_nodes: {result.json()['checkers'][0]['max_nodes']}\t equiv: {result.equivalence}")

    from qiskit import QuantumCircuit
    from qiskit.qasm3 import loads
    from qiskit.transpiler.passes import RemoveBarriers

    qc1 = QuantumCircuit(10, 3)

    qc1.x(5)
    qc1.cx(0, 7)
    qc1.cx(2, 7)
    qc1.cx(4, 7)
    qc1.cx(6, 7)

    qc1.cx(1, 8)
    qc1.cx(2, 8)
    qc1.cx(5, 8)
    qc1.cx(6, 8)

    qc1.cx(3, 9)
    qc1.cx(4, 9)
    qc1.cx(5, 9)
    qc1.cx(6, 9)

    qc1.measure([7, 8, 9], [0, 1, 2])

    output = """OPENQASM 3.0;
    include "stdgates.inc";
    bit[3] c;
    qubit[15] q;
    cx q[0], q[1];
    cx q[4], q[5];
    cx q[9], q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[5];
    cx q[9], q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[1];
    cx q[5], q[8];
    cx q[10], q[9];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[3];
    cx q[8], q[5];
    cx q[9], q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[3], q[2];
    cx q[5], q[8];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[3];
    cx q[7], q[8];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    rx(pi) q[2];
    cx q[3], q[11];
    cx q[5], q[8];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[1];
    cx q[8], q[9];
    cx q[11], q[3];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[5];
    cx q[3], q[11];
    cx q[9], q[8];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[3], q[11];
    cx q[5], q[2];
    cx q[8], q[9];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[5];
    cx q[7], q[8];
    cx q[9], q[10];
    cx q[11], q[3];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[2], q[1];
    cx q[3], q[11];
    cx q[5], q[8];
    cx q[10], q[9];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[0], q[1];
    cx q[2], q[5];
    cx q[9], q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[1], q[0];
    cx q[5], q[2];
    c[0] = measure q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[0], q[1];
    cx q[2], q[5];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[5], q[8];
    c[1] = measure q[0];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[4], q[5];
    cx q[8], q[9];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[5], q[4];
    cx q[9], q[8];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[4], q[5];
    cx q[8], q[9];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[9], q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[10], q[9];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    cx q[9], q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    c[2] = measure q[10];
    barrier q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8], q[9], q[10], q[11], q[12], q[13], q[14];
    """

    qc2 = loads(output)
    qc2 = RemoveBarriers()(qc2)

    result = qcec.verify(
        qc1,
        qc2,
        check_partial_equivalence=True,
        transform_dynamic_circuit=True,
        backpropagate_output_permutation=True,
        run_zx_checker=True,
        run_simulation_checker=False,
        run_alternating_checker=False,
        run_construction_checker=False,
    )

    print(result.equivalence)
