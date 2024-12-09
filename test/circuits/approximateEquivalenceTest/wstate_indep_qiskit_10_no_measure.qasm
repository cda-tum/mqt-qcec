// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[10];
creg meas[10];
ry(-pi/4) q[0];
ry(-0.9553166181245093) q[1];
ry(-pi/3) q[2];
ry(-1.1071487177940904) q[3];
ry(-1.1502619915109316) q[4];
ry(-1.183199640139716) q[5];
ry(-1.2094292028881888) q[6];
ry(-1.2309594173407747) q[7];
ry(-1.2490457723982544) q[8];
x q[9];
cz q[9],q[8];
ry(1.2490457723982544) q[8];
cz q[8],q[7];
ry(1.2309594173407747) q[7];
cz q[7],q[6];
ry(1.2094292028881888) q[6];
cz q[6],q[5];
ry(1.183199640139716) q[5];
cz q[5],q[4];
ry(1.1502619915109316) q[4];
cz q[4],q[3];
ry(1.1071487177940904) q[3];
cz q[3],q[2];
ry(pi/3) q[2];
cz q[2],q[1];
ry(0.9553166181245093) q[1];
cz q[1],q[0];
ry(pi/4) q[0];
cx q[8],q[9];
cx q[7],q[8];
cx q[6],q[7];
cx q[5],q[6];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
