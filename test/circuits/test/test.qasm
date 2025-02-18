// i 0 1
// o 0 1
OPENQASM 3.0;
include "stdgates.inc";
qubit[2] q;
cx q[1], q[0];
