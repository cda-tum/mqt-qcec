// i 0 1 2
// o 0 1 2
OPENQASM 3.0;
include "stdgates.inc";
qubit[3] q;
h q[1];
cx q[1], q[2];
ccx q[1], q[2], q[0];
cx q[2], q[1];
