OPENQASM 2.0;
include "qelib1.inc";
qreg q[1000];
h q;
CX q[0],q[1];
