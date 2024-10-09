OPENQASM 2.0;
include "qelib1.inc";

// This file was generated using the Berkeley Quantum Synthesis Toolkit (BQSKit) v1. (DOI: https://doi.org/10.11578/dc.20210603.2)

qreg q[2];
u3(1.5707963267948966, 0.0, 3.141592653589793) q[0];
cx q[0], q[1];
