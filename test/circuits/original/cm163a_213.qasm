// i 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
// o 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
OPENQASM 3.0;
include "stdgates.inc";
qubit[29] q;
cx q[13], q[5];
cx q[14], q[8];
x q[12];
x q[11];
x q[10];
x q[9];
x q[8];
x q[7];
x q[5];
x q[4];
ccx q[19], q[27], q[6];
cx q[15], q[7];
ctrl(3) @ x q[16], q[17], q[19], q[9];
ctrl(5) @ x q[19], q[24], q[25], q[26], q[27], q[6];
ctrl(7) @ x q[16], q[17], q[19], q[20], q[21], q[22], q[23], q[10];
ctrl(7) @ x q[16], q[17], q[19], q[20], q[21], q[22], q[23], q[9];
x q[18];
ctrl(3) @ x q[17], q[18], q[28], q[4];
ctrl(3) @ x q[17], q[18], q[28], q[3];
ctrl(8) @ x q[16], q[17], q[18], q[19], q[24], q[25], q[26], q[28], q[4];
ctrl(8) @ x q[16], q[17], q[18], q[19], q[24], q[25], q[26], q[28], q[2];
ctrl(4) @ x q[17], q[18], q[27], q[28], q[1];
ctrl(9) @ x q[16], q[17], q[18], q[19], q[24], q[25], q[26], q[27], q[28], q[1];
ctrl(4) @ x q[16], q[17], q[18], q[28], q[2];
x q[16];
ctrl(4) @ x q[16], q[17], q[18], q[28], q[1];
ctrl(3) @ x q[16], q[18], q[28], q[3];
x q[16];
x q[19];
ctrl(5) @ x q[16], q[17], q[18], q[19], q[28], q[2];
ctrl(5) @ x q[16], q[17], q[18], q[19], q[28], q[0];
ctrl(6) @ x q[16], q[17], q[18], q[19], q[27], q[28], q[1];
x q[19];
x q[17];
ccx q[16], q[17], q[11];
ctrl(7) @ x q[16], q[17], q[19], q[24], q[25], q[26], q[27], q[6];
x q[16];
x q[27];
ctrl(3) @ x q[16], q[17], q[27], q[12];
