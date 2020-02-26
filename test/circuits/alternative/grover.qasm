// i 0 2 1
// o 1 2 0
OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg c[3];

x q[1];
h q[0];
h q[2];
h q[1];

h q[1];
cx q[2],q[1];
tdg q[1];
cx q[0],q[1];
t q[1];
cx q[2],q[1];
tdg q[1];
cx q[0],q[1];
t q[2];
t q[1];
h q[1];
swap q[0],q[1];
cx q[1],q[2];
t q[1];
tdg q[2];
cx q[1],q[2];

h q[1];
h q[2];
x q[1];
x q[2];
h q[2];
cx q[1],q[2];
h q[2];
x q[2];
x q[1];
h q[2];
h q[1];
