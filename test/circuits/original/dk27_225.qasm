// i 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
// o 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
OPENQASM 3.0;
include "stdgates.inc";
qubit[18] q;
cx q[15], q[0];
cx q[13], q[8];
cx q[13], q[1];
x q[10];
ccx q[10], q[17], q[5];
ccx q[10], q[16], q[3];
cx q[10], q[8];
cx q[10], q[7];
ccx q[10], q[12], q[2];
x q[13];
ccx q[13], q[15], q[8];
x q[15];
ctrl(3) @ x q[10], q[13], q[15], q[8];
ctrl(3) @ x q[10], q[13], q[15], q[7];
x q[17];
x q[16];
x q[12];
ctrl(6) @ x q[10], q[12], q[13], q[15], q[16], q[17], q[6];
ctrl(6) @ x q[10], q[12], q[13], q[15], q[16], q[17], q[1];
x q[10];
x q[11];
ctrl(6) @ x q[10], q[11], q[12], q[13], q[15], q[16], q[6];
x q[14];
ctrl(5) @ x q[10], q[13], q[14], q[15], q[17], q[4];
