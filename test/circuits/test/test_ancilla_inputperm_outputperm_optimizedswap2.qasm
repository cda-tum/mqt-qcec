// i 0 2 1 3
// o 3 0
OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
creg c[2];
swap q[1],q[2];
cx q[0],q[1];
cx q[1],q[0];
swap q[1],q[3];

//cx q[3],q[1];
//cx q[1],q[3];
//id q[0];
//cx q[3],q[1];

//cx q[3],q[0];
//cx q[0],q[3];
//id q[0];
//cx q[3],q[0];

//cx q[0],q[1];
//cx q[1],q[0];
//id q[0];
//cx q[0],q[1];
