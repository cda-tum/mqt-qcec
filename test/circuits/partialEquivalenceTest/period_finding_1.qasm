OPENQASM 2.0;
include "qelib1.inc";
qreg q[8];
h q[0];
h q[1];
h q[2];
x q[7];
mcx q[0], q[7];
mcx q[0], q[7], q[4];
mcx q[0], q[7];
mcx q[0], q[4];
mcx q[0], q[4], q[7];
mcx q[0], q[4];
mcx q[0], q[5];
mcx q[0], q[4], q[5];
mcx q[0], q[4], q[6];
mcx q[0], q[5], q[7], q[4];
mcx q[0], q[5], q[6];
mcx q[0], q[6], q[5];
mcx q[0], q[4];
mcx q[0], q[5];
mcx q[0], q[4], q[5], q[7];
mcx q[0], q[4];
mcx q[0], q[5];
mcx q[0], q[7], q[5];
mcx q[0], q[4], q[5];
mcx q[0], q[4], q[6];
mcx q[0], q[4];
mcx q[0], q[4], q[5];
mcx q[0], q[6], q[7], q[4];
mcx q[0], q[4], q[7], q[6];
mcx q[0], q[4], q[5];
mcx q[0], q[5], q[4];
mcx q[0], q[6];
mcx q[0], q[6];
mcx q[0], q[4], q[7], q[6];
mcx q[0], q[4], q[5];
mcx q[0], q[4], q[6];
mcx q[0], q[5], q[6], q[4];
mcx q[0], q[5], q[6];
mcx q[0], q[7], q[4], q[5];
mcx q[0], q[5], q[6];
mcx q[0], q[7];
mcx q[0], q[6], q[7], q[4], q[5];
mcx q[0], q[7];
mcx q[0], q[6];
mcx q[0], q[4], q[7];
mcx q[0], q[4];
mcx q[0], q[4];
mcx q[0], q[5];
mcx q[0], q[4], q[5], q[6];
mcx q[0], q[4];
mcx q[0], q[5];
mcx q[0], q[6], q[4];
mcx q[0], q[5];
mcx q[0], q[4], q[5];
mcx q[0], q[5], q[4];
mcx q[0], q[5];
mcx q[0], q[4], q[6];
mcx q[0], q[4];
mcx q[0], q[4], q[5];
mcx q[1], q[7];
mcx q[1], q[7], q[4];
mcx q[1], q[7];
mcx q[1], q[4];
mcx q[1], q[4], q[7];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[5], q[7], q[4];
mcx q[1], q[5], q[6];
mcx q[1], q[6], q[5];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5], q[7];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[7], q[5];
mcx q[1], q[4], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[4];
mcx q[1], q[4], q[5];
mcx q[1], q[6], q[7], q[4];
mcx q[1], q[4], q[7], q[6];
mcx q[1], q[4], q[5];
mcx q[1], q[5], q[4];
mcx q[1], q[6];
mcx q[1], q[6];
mcx q[1], q[4], q[7], q[6];
mcx q[1], q[4], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[5], q[6], q[4];
mcx q[1], q[5], q[6];
mcx q[1], q[7], q[4], q[5];
mcx q[1], q[5], q[6];
mcx q[1], q[7];
mcx q[1], q[6], q[7], q[4], q[5];
mcx q[1], q[7];
mcx q[1], q[6];
mcx q[1], q[4], q[7];
mcx q[1], q[4];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5], q[6];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[6], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5];
mcx q[1], q[5], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[4];
mcx q[1], q[4], q[5];
mcx q[1], q[7];
mcx q[1], q[7], q[4];
mcx q[1], q[7];
mcx q[1], q[4];
mcx q[1], q[4], q[7];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[5], q[7], q[4];
mcx q[1], q[5], q[6];
mcx q[1], q[6], q[5];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5], q[7];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[7], q[5];
mcx q[1], q[4], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[4];
mcx q[1], q[4], q[5];
mcx q[1], q[6], q[7], q[4];
mcx q[1], q[4], q[7], q[6];
mcx q[1], q[4], q[5];
mcx q[1], q[5], q[4];
mcx q[1], q[6];
mcx q[1], q[6];
mcx q[1], q[4], q[7], q[6];
mcx q[1], q[4], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[5], q[6], q[4];
mcx q[1], q[5], q[6];
mcx q[1], q[7], q[4], q[5];
mcx q[1], q[5], q[6];
mcx q[1], q[7];
mcx q[1], q[6], q[7], q[4], q[5];
mcx q[1], q[7];
mcx q[1], q[6];
mcx q[1], q[4], q[7];
mcx q[1], q[4];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5], q[6];
mcx q[1], q[4];
mcx q[1], q[5];
mcx q[1], q[6], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[5];
mcx q[1], q[5], q[4];
mcx q[1], q[5];
mcx q[1], q[4], q[6];
mcx q[1], q[4];
mcx q[1], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[7], q[4];
mcx q[2], q[7];
mcx q[2], q[4];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[7], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[6], q[5];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[7], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[6], q[7], q[4];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[6];
mcx q[2], q[6];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[6], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[7], q[4], q[5];
mcx q[2], q[5], q[6];
mcx q[2], q[7];
mcx q[2], q[6], q[7], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[6];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[6];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[6], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[7], q[4];
mcx q[2], q[7];
mcx q[2], q[4];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[7], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[6], q[5];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[7], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[6], q[7], q[4];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[6];
mcx q[2], q[6];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[6], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[7], q[4], q[5];
mcx q[2], q[5], q[6];
mcx q[2], q[7];
mcx q[2], q[6], q[7], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[6];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[6];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[6], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[7], q[4];
mcx q[2], q[7];
mcx q[2], q[4];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[7], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[6], q[5];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[7], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[6], q[7], q[4];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[6];
mcx q[2], q[6];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[6], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[7], q[4], q[5];
mcx q[2], q[5], q[6];
mcx q[2], q[7];
mcx q[2], q[6], q[7], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[6];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[6];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[6], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[7], q[4];
mcx q[2], q[7];
mcx q[2], q[4];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[7], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[6], q[5];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[7];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[7], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
mcx q[2], q[6], q[7], q[4];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[6];
mcx q[2], q[6];
mcx q[2], q[4], q[7], q[6];
mcx q[2], q[4], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[5], q[6], q[4];
mcx q[2], q[5], q[6];
mcx q[2], q[7], q[4], q[5];
mcx q[2], q[5], q[6];
mcx q[2], q[7];
mcx q[2], q[6], q[7], q[4], q[5];
mcx q[2], q[7];
mcx q[2], q[6];
mcx q[2], q[4], q[7];
mcx q[2], q[4];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5], q[6];
mcx q[2], q[4];
mcx q[2], q[5];
mcx q[2], q[6], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[5];
mcx q[2], q[5], q[4];
mcx q[2], q[5];
mcx q[2], q[4], q[6];
mcx q[2], q[4];
mcx q[2], q[4], q[5];
h q[2];
cswap q[0], q[2], q[3];
t q[3];
cswap q[0], q[2], q[3];
cswap q[1], q[2], q[3];
s q[3];
cswap q[1], q[2], q[3];
h q[1];
cswap q[0], q[1], q[3];
s q[3];
cswap q[0], q[1], q[3];
h q[0];
swap q[0], q[2];
