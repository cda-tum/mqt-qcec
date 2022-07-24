// i 0 2 1
// o 0 2
OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg c[2];
swap q[1],q[2];
cx q[1],q[0];
swap q[1],q[2];
