// i 0 1 2
// o 0 1
OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg c[2];
swap q[1],q[2];
cx q[2],q[0];
swap q[1],q[2];

