// Benchmark was created by MQT Bench on 2024-03-17
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[10];
creg meas[10];
h q[0];
h q[1];
cz q[0],q[1];
h q[2];
cz q[1],q[2];
h q[3];
h q[4];
cz q[0],q[4];
cz q[3],q[4];
h q[5];
cz q[2],q[5];
h q[6];
cz q[3],q[6];
cz q[5],q[6];
h q[7];
h q[8];
cz q[7],q[8];
h q[9];
cz q[7],q[9];
cz q[8],q[9];
