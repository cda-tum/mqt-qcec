// i 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
// o 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
OPENQASM 3.0;
include "stdgates.inc";
qubit[19] q;
ctrl(3) @ x q[9], q[10], q[16], q[2];
ccx q[10], q[16], q[4];
ccx q[10], q[16], q[3];
ccx q[11], q[17], q[5];
ccx q[11], q[17], q[4];
ccx q[11], q[17], q[3];
ccx q[9], q[15], q[3];
ccx q[9], q[15], q[2];
ccx q[9], q[15], q[1];
ccx q[9], q[15], q[0];
ctrl(3) @ x q[8], q[9], q[15], q[1];
ctrl(3) @ x q[8], q[9], q[15], q[0];
ctrl(3) @ x q[7], q[8], q[14], q[0];
ctrl(3) @ x q[10], q[15], q[16], q[2];
ctrl(3) @ x q[10], q[15], q[16], q[1];
ctrl(4) @ x q[10], q[14], q[15], q[16], q[1];
ctrl(3) @ x q[11], q[12], q[18], q[4];
ctrl(3) @ x q[11], q[12], q[18], q[3];
ctrl(3) @ x q[11], q[12], q[18], q[2];
ctrl(3) @ x q[11], q[12], q[18], q[1];
ctrl(3) @ x q[10], q[11], q[17], q[3];
ctrl(3) @ x q[8], q[13], q[14], q[0];
ccx q[8], q[14], q[2];
ccx q[8], q[14], q[1];
ccx q[7], q[13], q[1];
ccx q[7], q[13], q[0];
x q[16];
ctrl(5) @ x q[8], q[11], q[15], q[16], q[17], q[1];
ctrl(5) @ x q[8], q[9], q[11], q[16], q[17], q[1];
ctrl(3) @ x q[11], q[16], q[17], q[3];
ctrl(5) @ x q[9], q[11], q[12], q[16], q[18], q[2];
ctrl(5) @ x q[9], q[11], q[12], q[16], q[18], q[1];
ctrl(5) @ x q[9], q[12], q[16], q[17], q[18], q[2];
ctrl(5) @ x q[9], q[12], q[16], q[17], q[18], q[1];
x q[10];
ctrl(7) @ x q[7], q[10], q[11], q[12], q[14], q[15], q[18], q[0];
ctrl(7) @ x q[7], q[10], q[12], q[14], q[15], q[17], q[18], q[0];
ctrl(5) @ x q[8], q[9], q[10], q[11], q[17], q[1];
ctrl(5) @ x q[8], q[10], q[11], q[15], q[17], q[1];
ccx q[10], q[16], q[4];
x q[17];
ctrl(3) @ x q[12], q[17], q[18], q[4];
ctrl(6) @ x q[10], q[12], q[14], q[15], q[17], q[18], q[1];
ctrl(7) @ x q[7], q[8], q[10], q[12], q[15], q[17], q[18], q[0];
ctrl(5) @ x q[9], q[10], q[12], q[17], q[18], q[2];
ctrl(5) @ x q[9], q[10], q[12], q[17], q[18], q[1];
ctrl(5) @ x q[9], q[10], q[12], q[17], q[18], q[0];
ctrl(6) @ x q[8], q[9], q[10], q[12], q[17], q[18], q[1];
ctrl(5) @ x q[10], q[12], q[15], q[17], q[18], q[2];
ctrl(5) @ x q[10], q[12], q[15], q[17], q[18], q[1];
ctrl(4) @ x q[10], q[12], q[17], q[18], q[3];
ctrl(7) @ x q[9], q[10], q[12], q[13], q[14], q[17], q[18], q[0];
x q[10];
ctrl(4) @ x q[12], q[16], q[17], q[18], q[3];
ctrl(4) @ x q[12], q[16], q[17], q[18], q[0];
ctrl(5) @ x q[12], q[15], q[16], q[17], q[18], q[2];
ctrl(5) @ x q[12], q[15], q[16], q[17], q[18], q[1];
ctrl(5) @ x q[12], q[15], q[16], q[17], q[18], q[0];
x q[11];
ctrl(4) @ x q[10], q[11], q[12], q[18], q[3];
ctrl(5) @ x q[10], q[11], q[12], q[15], q[18], q[2];
ctrl(5) @ x q[10], q[11], q[12], q[15], q[18], q[1];
x q[10];
ctrl(5) @ x q[9], q[10], q[11], q[12], q[18], q[2];
ctrl(6) @ x q[10], q[11], q[12], q[14], q[15], q[18], q[1];
ctrl(7) @ x q[7], q[9], q[10], q[11], q[12], q[14], q[18], q[0];
ctrl(7) @ x q[7], q[8], q[10], q[11], q[12], q[15], q[18], q[0];
ctrl(7) @ x q[8], q[10], q[11], q[12], q[13], q[15], q[18], q[0];
ctrl(4) @ x q[11], q[12], q[16], q[18], q[3];
ctrl(4) @ x q[11], q[12], q[16], q[18], q[2];
ctrl(4) @ x q[11], q[12], q[16], q[18], q[1];
ccx q[11], q[17], q[5];
x q[13];
ctrl(4) @ x q[9], q[13], q[14], q[15], q[0];
ctrl(4) @ x q[8], q[9], q[13], q[15], q[0];
ctrl(7) @ x q[8], q[9], q[10], q[12], q[13], q[17], q[18], q[0];
x q[10];
ctrl(7) @ x q[8], q[9], q[10], q[11], q[12], q[13], q[18], q[0];
ctrl(7) @ x q[8], q[11], q[12], q[13], q[15], q[16], q[18], q[0];
x q[16];
ctrl(7) @ x q[8], q[9], q[11], q[12], q[13], q[16], q[18], q[0];
x q[14];
ctrl(4) @ x q[9], q[10], q[14], q[16], q[1];
ctrl(3) @ x q[9], q[14], q[15], q[1];
x q[10];
ctrl(7) @ x q[10], q[12], q[13], q[14], q[15], q[17], q[18], q[0];
ctrl(6) @ x q[10], q[12], q[14], q[15], q[17], q[18], q[0];
ctrl(7) @ x q[7], q[9], q[10], q[12], q[14], q[17], q[18], q[0];
ctrl(6) @ x q[9], q[10], q[12], q[14], q[17], q[18], q[1];
ctrl(6) @ x q[9], q[10], q[11], q[12], q[14], q[18], q[1];
x q[10];
ctrl(5) @ x q[9], q[10], q[13], q[14], q[16], q[0];
ctrl(5) @ x q[10], q[13], q[14], q[15], q[16], q[0];
x q[16];
ctrl(7) @ x q[9], q[12], q[13], q[14], q[16], q[17], q[18], q[0];
ctrl(7) @ x q[12], q[13], q[14], q[15], q[16], q[17], q[18], q[0];
x q[7];
x q[10];
ctrl(7) @ x q[7], q[8], q[9], q[10], q[11], q[12], q[18], q[0];
ccx q[7], q[13], q[1];
x q[13];
ctrl(4) @ x q[7], q[9], q[14], q[15], q[0];
x q[8];
ctrl(6) @ x q[8], q[11], q[12], q[15], q[16], q[18], q[1];
ctrl(6) @ x q[8], q[9], q[11], q[12], q[16], q[18], q[1];
ctrl(6) @ x q[8], q[9], q[12], q[16], q[17], q[18], q[1];
ctrl(6) @ x q[8], q[9], q[12], q[16], q[17], q[18], q[0];
ctrl(6) @ x q[8], q[12], q[15], q[16], q[17], q[18], q[1];
ctrl(6) @ x q[8], q[12], q[15], q[16], q[17], q[18], q[0];
ctrl(7) @ x q[8], q[10], q[12], q[13], q[15], q[17], q[18], q[0];
ctrl(6) @ x q[8], q[10], q[12], q[15], q[17], q[18], q[1];
ctrl(4) @ x q[7], q[8], q[9], q[15], q[0];
ctrl(3) @ x q[8], q[9], q[15], q[0];
ctrl(7) @ x q[7], q[8], q[11], q[12], q[15], q[16], q[18], q[0];
ctrl(7) @ x q[7], q[8], q[12], q[15], q[16], q[17], q[18], q[0];
ctrl(7) @ x q[7], q[8], q[9], q[12], q[16], q[17], q[18], q[0];
ctrl(7) @ x q[7], q[8], q[9], q[10], q[12], q[17], q[18], q[0];
x q[17];
ctrl(7) @ x q[7], q[8], q[9], q[11], q[12], q[16], q[18], q[0];
x q[11];
ccx q[8], q[14], q[2];
x q[15];
ctrl(4) @ x q[10], q[11], q[15], q[17], q[2];
ctrl(5) @ x q[7], q[10], q[11], q[15], q[17], q[0];
ctrl(6) @ x q[7], q[11], q[14], q[15], q[16], q[17], q[0];
x q[7];
ctrl(6) @ x q[7], q[8], q[10], q[11], q[15], q[17], q[0];
ctrl(5) @ x q[10], q[11], q[14], q[15], q[17], q[0];
ctrl(6) @ x q[10], q[11], q[13], q[14], q[15], q[17], q[0];
ctrl(4) @ x q[11], q[15], q[16], q[17], q[2];
x q[14];
ctrl(5) @ x q[10], q[11], q[14], q[15], q[17], q[1];
ctrl(5) @ x q[11], q[14], q[15], q[16], q[17], q[1];
ctrl(6) @ x q[7], q[8], q[11], q[15], q[16], q[17], q[0];
ctrl(6) @ x q[8], q[11], q[13], q[15], q[16], q[17], q[0];
x q[16];
ctrl(6) @ x q[8], q[10], q[11], q[13], q[15], q[17], q[0];
x q[10];
ctrl(4) @ x q[8], q[10], q[15], q[16], q[1];
x q[7];
ctrl(5) @ x q[7], q[10], q[14], q[15], q[16], q[0];
ctrl(6) @ x q[7], q[10], q[11], q[14], q[15], q[17], q[0];
ctrl(5) @ x q[7], q[11], q[14], q[15], q[17], q[0];
x q[13];
ctrl(5) @ x q[7], q[8], q[10], q[15], q[16], q[0];
ctrl(4) @ x q[7], q[10], q[15], q[16], q[0];
ctrl(5) @ x q[8], q[10], q[13], q[15], q[16], q[0];
x q[16];
ctrl(6) @ x q[11], q[13], q[14], q[15], q[16], q[17], q[0];
ctrl(5) @ x q[11], q[13], q[15], q[16], q[17], q[0];
x q[16];
x q[13];
x q[11];
ctrl(5) @ x q[11], q[12], q[15], q[16], q[18], q[2];
ctrl(5) @ x q[11], q[12], q[15], q[16], q[18], q[1];
ctrl(7) @ x q[10], q[11], q[12], q[13], q[14], q[15], q[18], q[0];
ctrl(7) @ x q[11], q[12], q[13], q[14], q[15], q[16], q[18], q[0];
ctrl(6) @ x q[8], q[10], q[11], q[12], q[15], q[18], q[1];
ctrl(5) @ x q[8], q[11], q[12], q[15], q[18], q[1];
x q[16];
ctrl(6) @ x q[11], q[12], q[14], q[15], q[16], q[18], q[1];
ctrl(6) @ x q[11], q[12], q[14], q[15], q[16], q[18], q[0];
ctrl(7) @ x q[7], q[11], q[12], q[14], q[15], q[16], q[18], q[0];
ctrl(6) @ x q[7], q[11], q[12], q[15], q[16], q[18], q[0];
x q[11];
x q[17];
ctrl(6) @ x q[12], q[14], q[15], q[16], q[17], q[18], q[1];
ctrl(7) @ x q[8], q[12], q[13], q[15], q[16], q[17], q[18], q[0];
ctrl(7) @ x q[7], q[12], q[14], q[15], q[16], q[17], q[18], q[0];
x q[7];
ctrl(6) @ x q[7], q[12], q[15], q[16], q[17], q[18], q[0];
x q[17];
x q[9];
ctrl(4) @ x q[9], q[11], q[16], q[17], q[2];
ctrl(6) @ x q[7], q[8], q[9], q[11], q[16], q[17], q[0];
ctrl(5) @ x q[7], q[9], q[11], q[16], q[17], q[0];
ctrl(6) @ x q[7], q[9], q[11], q[14], q[16], q[17], q[0];
ctrl(5) @ x q[9], q[11], q[14], q[16], q[17], q[1];
ctrl(6) @ x q[8], q[9], q[11], q[13], q[16], q[17], q[0];
x q[16];
ctrl(5) @ x q[8], q[9], q[10], q[13], q[16], q[0];
ctrl(4) @ x q[8], q[9], q[10], q[16], q[1];
ctrl(4) @ x q[8], q[9], q[10], q[16], q[0];
x q[8];
ccx q[9], q[15], q[3];
x q[7];
ctrl(5) @ x q[7], q[8], q[9], q[10], q[16], q[0];
ctrl(5) @ x q[7], q[9], q[10], q[14], q[16], q[0];
x q[10];
ctrl(4) @ x q[9], q[10], q[11], q[17], q[2];
ctrl(5) @ x q[9], q[10], q[11], q[14], q[17], q[1];
ctrl(5) @ x q[9], q[10], q[11], q[14], q[17], q[0];
ctrl(6) @ x q[7], q[8], q[9], q[10], q[11], q[17], q[0];
ctrl(6) @ x q[7], q[9], q[10], q[11], q[14], q[17], q[0];
ctrl(5) @ x q[8], q[9], q[10], q[11], q[17], q[0];
x q[8];
ctrl(6) @ x q[8], q[9], q[10], q[11], q[13], q[17], q[0];
x q[14];
ctrl(6) @ x q[9], q[11], q[13], q[14], q[16], q[17], q[0];
ctrl(6) @ x q[9], q[10], q[11], q[13], q[14], q[17], q[0];
ctrl(5) @ x q[9], q[11], q[13], q[14], q[17], q[0];
x q[14];
x q[11];
x q[16];
ctrl(6) @ x q[9], q[11], q[12], q[14], q[16], q[18], q[1];
ctrl(6) @ x q[9], q[11], q[12], q[14], q[16], q[18], q[0];
ctrl(7) @ x q[9], q[11], q[12], q[13], q[14], q[16], q[18], q[0];
ctrl(7) @ x q[9], q[10], q[11], q[12], q[13], q[14], q[18], q[0];
ctrl(6) @ x q[8], q[9], q[10], q[11], q[12], q[18], q[1];
x q[17];
ctrl(6) @ x q[9], q[12], q[14], q[16], q[17], q[18], q[1];
ctrl(7) @ x q[8], q[9], q[12], q[13], q[16], q[17], q[18], q[0];
x q[14];
ctrl(7) @ x q[7], q[9], q[11], q[12], q[14], q[16], q[18], q[0];
ctrl(7) @ x q[7], q[9], q[12], q[14], q[16], q[17], q[18], q[0];
x q[12];
cx q[12], q[6];
ccx q[12], q[18], q[5];
ccx q[12], q[18], q[4];
ccx q[12], q[18], q[3];
ccx q[12], q[18], q[2];
ccx q[12], q[18], q[1];
x q[18];
cx q[18], q[6];
cx q[18], q[5];
cx q[18], q[4];
cx q[18], q[3];
cx q[18], q[2];
cx q[18], q[1];
