// i 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
// o 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
OPENQASM 3.0;
include "stdgates.inc";
qubit[25] q;
x q[10];
ccx q[11], q[18], q[1];
x q[21];
ctrl(4) @ x q[18], q[19], q[21], q[22], q[0];
ccx q[18], q[21], q[0];
x q[22];
x q[12];
ctrl(6) @ x q[11], q[12], q[19], q[20], q[21], q[22], q[2];
x q[17];
x q[13];
ctrl(10) @ x q[11], q[12], q[13], q[17], q[19], q[20], q[21], q[22], q[23], q[24], q[4];
x q[15];
ctrl(10) @ x q[11], q[12], q[15], q[17], q[19], q[20], q[21], q[22], q[23], q[24], q[4];
ctrl(9) @ x q[11], q[12], q[15], q[17], q[19], q[20], q[21], q[22], q[24], q[4];
x q[24];
x q[14];
ctrl(10) @ x q[11], q[12], q[14], q[17], q[19], q[20], q[21], q[22], q[23], q[24], q[4];
x q[23];
x q[16];
ctrl(10) @ x q[11], q[12], q[16], q[17], q[19], q[20], q[21], q[22], q[23], q[24], q[4];
x q[20];
ctrl(4) @ x q[19], q[20], q[21], q[22], q[10];
ctrl(4) @ x q[19], q[20], q[21], q[22], q[9];
x q[11];
ctrl(5) @ x q[11], q[19], q[20], q[21], q[22], q[3];
ctrl(6) @ x q[11], q[19], q[20], q[21], q[22], q[23], q[7];
ctrl(7) @ x q[11], q[19], q[20], q[21], q[22], q[23], q[24], q[8];
ctrl(7) @ x q[11], q[19], q[20], q[21], q[22], q[23], q[24], q[7];
x q[23];
ctrl(6) @ x q[11], q[19], q[20], q[21], q[22], q[23], q[5];
ctrl(7) @ x q[11], q[19], q[20], q[21], q[22], q[23], q[24], q[6];
ctrl(7) @ x q[11], q[19], q[20], q[21], q[22], q[23], q[24], q[5];
x q[20];
x q[22];
x q[19];
ctrl(4) @ x q[19], q[20], q[21], q[22], q[10];
ctrl(4) @ x q[19], q[20], q[21], q[22], q[9];
ctrl(5) @ x q[11], q[19], q[20], q[21], q[22], q[4];
ctrl(5) @ x q[11], q[19], q[20], q[21], q[22], q[2];
x q[11];
