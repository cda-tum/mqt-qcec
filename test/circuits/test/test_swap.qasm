// i 0 1
// o 0 1
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
swap q[0],q[1];
cx q[0],q[1];
swap q[0],q[1];

