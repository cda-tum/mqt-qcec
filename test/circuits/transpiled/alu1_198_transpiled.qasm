// i 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52
// o 4 9 22 10 24 16 2 0 1 5 13 6 12 8 15 14 17 7 3 23 11
OPENQASM 2.0;
include "qelib1.inc";
qreg q[53];
creg c[20];
u2(0.0,3.141592653589793) q[0];
u2(0.0,3.141592653589793) q[1];
u2(0.0,3.141592653589793) q[2];
u2(0.0,3.141592653589793) q[3];
u2(0.0,3.141592653589793) q[4];
u3(1.5707963267948968,3.141592653589793,3.141592653589793) q[5];
u3(1.5707963267948968,3.141592653589793,3.141592653589793) q[6];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
u3(1.5707963267948968,3.141592653589793,3.141592653589793) q[7];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
u3(3.141592653589793,0.0,3.141592653589793) q[10];
u3(3.141592653589793,0.0,3.141592653589793) q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[15],q[18];
cx q[18],q[15];
cx q[15],q[18];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
u2(0.7853981633974483,3.141592653589793) q[20];
cx q[20],q[21];
cx q[21],q[20];
cx q[20],q[21];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[23],q[22];
u1(-0.7853981633974483) q[22];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[20],q[21];
cx q[21],q[20];
cx q[20],q[21];
cx q[19],q[20];
u1(0.7853981633974483) q[20];
cx q[20],q[21];
cx q[21],q[20];
cx q[20],q[21];
cx q[19],q[20];
cx q[20],q[19];
cx q[19],q[20];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[22],q[21];
u2(0.0,2.356194490192345) q[21];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[17],q[23];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[20],q[21];
cx q[21],q[20];
cx q[20],q[21];
cx q[19],q[20];
cx q[20],q[19];
cx q[19],q[20];
cx q[23],q[17];
cx q[17],q[23];
cx q[17],q[11];
u1(-0.7853981633974483) q[11];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[9],q[10];
u1(0.7853981633974483) q[10];
cx q[11],q[10];
u1(-0.7853981633974483) q[10];
u1(0.7853981633974483) q[11];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u1(3.9269908169872414) q[11];
cx q[9],q[10];
u1(-0.7853981633974483) q[10];
u1(0.7853981633974483) q[9];
cx q[9],q[10];
u2(0.7853981633974483,3.141592653589793) q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[23],q[17];
u1(-0.7853981633974483) q[17];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[17],q[23];
cx q[21],q[22];
u1(0.7853981633974483) q[22];
cx q[23],q[17];
cx q[17],q[23];
cx q[22],q[23];
cx q[23],q[22];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[12],q[11];
u1(-0.7853981633974483) q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
u1(0.7853981633974483) q[12];
cx q[11],q[12];
u2(0.0,2.356194490192345) q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[14],q[15];
cx q[15],q[14];
cx q[14],q[15];
cx q[15],q[18];
cx q[18],q[15];
cx q[15],q[18];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[20],q[21];
cx q[21],q[20];
cx q[20],q[21];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[10],q[9];
u1(-0.7853981633974483) q[9];
cx q[5],q[9];
u1(0.7853981633974483) q[9];
cx q[10],q[9];
u1(0.7853981633974483) q[10];
u1(-0.7853981633974483) q[9];
cx q[5],q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
u1(7.0685834705770345) q[10];
cx q[5],q[9];
u1(0.7853981633974483) q[5];
u1(-0.7853981633974483) q[9];
cx q[5],q[9];
cx q[0],q[5];
cx q[5],q[0];
cx q[0],q[5];
cx q[0],q[1];
cx q[1],q[0];
cx q[0],q[1];
u2(0.7853981633974483,3.141592653589793) q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[11],q[10];
u1(-0.7853981633974483) q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
u1(0.7853981633974483) q[11];
cx q[10],q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
u1(-0.7853981633974483) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[14],q[13];
u1(0.7853981633974483) q[13];
cx q[12],q[13];
u2(0.0,2.356194490192345) q[13];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[6],q[4];
u1(-0.7853981633974483) q[4];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
u1(0.7853981633974483) q[2];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[4],q[3];
u1(-0.7853981633974483) q[3];
cx q[3],q[2];
cx q[2],q[3];
u1(7.0685834705770345) q[2];
u1(0.7853981633974483) q[4];
cx q[3],q[4];
u1(0.7853981633974483) q[3];
u1(-0.7853981633974483) q[4];
cx q[3],q[4];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
u2(0.7853981633974483,3.141592653589793) q[4];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[13],q[6];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
u1(-0.7853981633974483) q[6];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[14],q[13];
u1(0.7853981633974483) q[13];
cx q[6],q[13];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[15],q[14];
u1(-0.7853981633974483) q[14];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[14],q[15];
cx q[15],q[14];
cx q[14],q[15];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
u1(0.7853981633974483) q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[14],q[13];
u2(0.0,2.356194490192345) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[14],q[15];
cx q[15],q[14];
cx q[14],q[15];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[10],q[9];
u1(-0.7853981633974483) q[9];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[0],q[5];
cx q[5],q[0];
cx q[0],q[5];
cx q[1],q[0];
u1(0.7853981633974483) q[0];
cx q[0],q[5];
cx q[5],q[0];
cx q[0],q[5];
cx q[0],q[1];
cx q[1],q[0];
cx q[0],q[1];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[9],q[5];
u1(-0.7853981633974483) q[5];
cx q[0],q[5];
u1(0.7853981633974483) q[9];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[0],q[5];
u1(0.7853981633974483) q[0];
u1(-0.7853981633974483) q[5];
cx q[0],q[5];
u2(0.7853981633974483,3.141592653589793) q[5];
u1(7.0685834705770345) q[9];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
u1(-0.7853981633974483) q[11];
cx q[10],q[11];
u1(0.7853981633974483) q[11];
cx q[12],q[11];
cx q[11],q[17];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[17],q[11];
cx q[11],q[17];
cx q[23],q[17];
u1(-0.7853981633974483) q[17];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[22],q[23];
u1(0.7853981633974483) q[23];
cx q[17],q[23];
u2(0.0,2.356194490192345) q[23];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[23],q[24];
cx q[24],q[23];
cx q[23],q[24];
cx q[24],q[25];
cx q[25],q[24];
cx q[24],q[25];
cx q[18],q[27];
cx q[27],q[18];
cx q[18],q[27];
cx q[27],q[26];
cx q[26],q[27];
cx q[27],q[26];
cx q[25],q[26];
u1(-0.7853981633974483) q[26];
cx q[25],q[26];
cx q[26],q[25];
cx q[25],q[26];
cx q[24],q[25];
cx q[25],q[24];
cx q[24],q[25];
cx q[23],q[24];
cx q[24],q[23];
cx q[23],q[24];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[21],q[22];
u1(0.7853981633974483) q[22];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[25],q[26];
cx q[26],q[25];
cx q[25],q[26];
cx q[24],q[25];
cx q[25],q[24];
cx q[24],q[25];
cx q[24],q[23];
u1(-0.7853981633974483) q[23];
cx q[22],q[23];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
u1(7.0685834705770345) q[22];
u1(0.7853981633974483) q[24];
cx q[23],q[24];
u1(0.7853981633974483) q[23];
u1(-0.7853981633974483) q[24];
cx q[23],q[24];
cx q[23],q[22];
cx q[17],q[23];
u1(-0.7853981633974483) q[22];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[23],q[17];
cx q[17],q[23];
u2(0.7853981633974483,3.141592653589793) q[24];
cx q[23],q[24];
u1(-0.7853981633974483) q[24];
cx q[23],q[24];
cx q[24],q[23];
cx q[23],q[24];
cx q[22],q[23];
cx q[22],q[21];
u1(0.7853981633974483) q[21];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
u1(0.7853981633974483) q[23];
cx q[24],q[23];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[23],q[22];
u1(-0.7853981633974483) q[22];
cx q[21],q[22];
u1(0.7853981633974483) q[23];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
cx q[21],q[22];
u1(0.7853981633974483) q[21];
u1(-0.7853981633974483) q[22];
cx q[21],q[22];
u3(3.141592653589793,0.0,3.141592653589793) q[21];
cx q[21],q[22];
cx q[22],q[21];
cx q[21],q[22];
cx q[20],q[21];
cx q[21],q[20];
cx q[20],q[21];
u2(0.0,3.9269908169872414) q[23];
cx q[23],q[24];
cx q[24],q[23];
cx q[23],q[24];
cx q[9],q[10];
u1(-0.7853981633974483) q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
u1(0.7853981633974483) q[11];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
u2(0.0,2.356194490192345) q[11];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[9],q[8];
u1(-0.7853981633974483) q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
cx q[20],q[19];
u1(0.7853981633974483) q[19];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
cx q[19],q[20];
cx q[20],q[19];
cx q[19],q[20];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[8],q[7];
u1(-0.7853981633974483) q[7];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
cx q[19],q[16];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
u1(7.0685834705770345) q[19];
u1(0.7853981633974483) q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[16],q[7];
u1(0.7853981633974483) q[16];
u1(-0.7853981633974483) q[7];
cx q[16],q[7];
cx q[16],q[19];
u1(-0.7853981633974483) q[19];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
u2(0.7853981633974483,3.141592653589793) q[7];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
cx q[16],q[19];
cx q[19],q[16];
cx q[16],q[19];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[9],q[8];
u1(-0.7853981633974483) q[8];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
u1(0.7853981633974483) q[10];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[8],q[7];
u1(0.7853981633974483) q[7];
cx q[16],q[7];
u1(0.7853981633974483) q[16];
u1(-0.7853981633974483) q[7];
cx q[8],q[7];
cx q[7],q[16];
cx q[16],q[7];
cx q[7],q[16];
u2(0.0,3.9269908169872414) q[16];
cx q[8],q[7];
u1(-0.7853981633974483) q[7];
u1(0.7853981633974483) q[8];
cx q[8],q[7];
u3(3.141592653589793,0.0,3.141592653589793) q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
u1(-0.7853981633974483) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[14],q[13];
u1(0.7853981633974483) q[13];
cx q[12],q[13];
u2(0.0,2.356194490192345) q[13];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[6],q[4];
u1(-0.7853981633974483) q[4];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[14],q[15];
cx q[15],q[14];
cx q[14],q[15];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[0],q[5];
cx q[5],q[0];
cx q[0],q[5];
cx q[0],q[1];
u1(0.7853981633974483) q[1];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[3],q[2];
u1(-0.7853981633974483) q[2];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[1],q[0];
cx q[0],q[1];
u1(7.0685834705770345) q[0];
u1(0.7853981633974483) q[3];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
u1(0.7853981633974483) q[1];
u1(-0.7853981633974483) q[2];
cx q[1],q[2];
cx q[1],q[0];
u1(-0.7853981633974483) q[0];
cx q[0],q[1];
cx q[1],q[0];
cx q[0],q[1];
u2(0.7853981633974483,3.141592653589793) q[2];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[0],q[1];
cx q[1],q[0];
cx q[0],q[1];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[6],q[4];
u1(-0.7853981633974483) q[4];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
u1(0.7853981633974483) q[13];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[4],q[3];
u1(0.7853981633974483) q[3];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
u1(0.7853981633974483) q[1];
u1(-0.7853981633974483) q[2];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[4],q[3];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[3],q[2];
u1(-0.7853981633974483) q[2];
u1(0.7853981633974483) q[3];
cx q[3],q[2];
cx q[1],q[2];
cx q[2],q[1];
cx q[1],q[2];
cx q[0],q[1];
cx q[1],q[0];
cx q[0],q[1];
u3(3.141592653589793,0.0,3.141592653589793) q[3];
u2(0.0,3.9269908169872414) q[4];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[6],q[13];
cx q[12],q[13];
u1(-0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
u2(0.0,2.356194490192345) q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[11],q[10];
u1(-0.7853981633974483) q[10];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[0],q[5];
u1(0.7853981633974483) q[5];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[0],q[5];
cx q[10],q[9];
u1(0.7853981633974483) q[10];
cx q[5],q[0];
cx q[0],q[5];
cx q[0],q[1];
cx q[1],q[0];
cx q[0],q[1];
u1(-0.7853981633974483) q[9];
cx q[9],q[5];
cx q[5],q[9];
u1(7.0685834705770345) q[5];
cx q[9],q[10];
u1(-0.7853981633974483) q[10];
u1(0.7853981633974483) q[9];
cx q[9],q[10];
u2(0.7853981633974483,3.141592653589793) q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
u1(-0.7853981633974483) q[11];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[9],q[5];
u1(-0.7853981633974483) q[5];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[10],q[9];
u1(0.7853981633974483) q[9];
cx q[5],q[9];
u1(0.7853981633974483) q[5];
u1(-0.7853981633974483) q[9];
cx q[10],q[9];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[10],q[9];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u1(0.7853981633974483) q[11];
u2(0.0,3.9269908169872414) q[5];
cx q[0],q[5];
cx q[5],q[0];
cx q[0],q[5];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
u1(-0.7853981633974483) q[8];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u1(-0.7853981633974483) q[11];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[10],q[9];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u3(3.141592653589793,0.0,3.141592653589793) q[11];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[8],q[9];
u1(0.7853981633974483) q[9];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[7],q[8];
u2(0.0,2.356194490192345) q[8];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[9],q[5];
u1(-0.7853981633974483) q[5];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
u1(0.7853981633974483) q[11];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[5],q[9];
cx q[9],q[5];
cx q[5],q[9];
cx q[9],q[10];
u1(-0.7853981633974483) q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
u1(7.0685834705770345) q[12];
u1(0.7853981633974483) q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[11],q[10];
u1(-0.7853981633974483) q[10];
u1(0.7853981633974483) q[11];
cx q[11],q[10];
u2(0.7853981633974483,3.141592653589793) q[10];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[8],q[9];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
u1(-0.7853981633974483) q[9];
cx q[8],q[9];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
u1(0.7853981633974483) q[9];
cx q[8],q[9];
u3(3.141592653589793,0.0,3.141592653589793) q[8];
cx q[8],q[9];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
u1(-0.7853981633974483) q[9];
cx q[8],q[9];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
u1(0.7853981633974483) q[9];
cx q[8],q[9];
u2(0.0,2.356194490192345) q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
u1(-0.7853981633974483) q[11];
cx q[17],q[11];
u1(0.7853981633974483) q[11];
cx q[10],q[11];
u1(0.7853981633974483) q[10];
u1(-0.7853981633974483) q[11];
cx q[17],q[11];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u2(0.0,3.9269908169872414) q[10];
cx q[17],q[11];
u1(-0.7853981633974483) q[11];
u1(0.7853981633974483) q[17];
cx q[17],q[11];
u2(0.7853981633974483,3.141592653589793) q[11];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[9],q[10];
u1(-0.7853981633974483) q[10];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[8],q[9];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
u1(0.7853981633974483) q[9];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[12],q[11];
u1(-0.7853981633974483) q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[6],q[13];
u1(0.7853981633974483) q[13];
cx q[12],q[13];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
u2(0.0,2.356194490192345) q[13];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[4],q[3];
u1(-0.7853981633974483) q[3];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[13],q[6];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
u1(0.7853981633974483) q[6];
cx q[4],q[6];
u1(0.7853981633974483) q[4];
u1(-0.7853981633974483) q[6];
cx q[13],q[6];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[13],q[6];
u1(0.7853981633974483) q[13];
u1(7.0685834705770345) q[4];
u1(-0.7853981633974483) q[6];
cx q[13],q[6];
u2(0.7853981633974483,3.141592653589793) q[6];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[12],q[13];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
u1(-0.7853981633974483) q[13];
cx q[12],q[13];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
u1(0.7853981633974483) q[13];
cx q[12],q[13];
u3(3.141592653589793,0.0,3.141592653589793) q[12];
cx q[12],q[13];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
u1(-0.7853981633974483) q[13];
cx q[12],q[13];
u1(0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[11],q[17];
u2(0.0,2.356194490192345) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[17],q[11];
cx q[11],q[17];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[13],q[6];
u1(-0.7853981633974483) q[6];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[6],q[13];
u1(-0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[12],q[11];
cx q[11],q[12];
u2(0.0,3.9269908169872414) q[11];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[23],q[22];
cx q[22],q[23];
cx q[23],q[22];
u1(0.7853981633974483) q[6];
cx q[6],q[13];
cx q[13],q[6];
cx q[6],q[13];
cx q[12],q[13];
u1(0.7853981633974483) q[12];
u1(-0.7853981633974483) q[13];
cx q[12],q[13];
u2(0.7853981633974483,3.141592653589793) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(-0.7853981633974483) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[6],q[13];
u1(0.7853981633974483) q[13];
cx q[12],q[13];
cx q[14],q[13];
u1(-0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[14],q[15];
cx q[15],q[14];
cx q[14],q[15];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[9],q[8];
cx q[8],q[9];
cx q[9],q[8];
cx q[9],q[10];
cx q[10],q[9];
cx q[9],q[10];
cx q[10],q[11];
u1(0.7853981633974483) q[11];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
cx q[13],q[12];
u2(0.0,2.356194490192345) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[13],q[14];
u1(-0.7853981633974483) q[14];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[6],q[13];
u1(0.7853981633974483) q[13];
cx q[14],q[13];
u1(-0.7853981633974483) q[13];
u1(0.7853981633974483) q[14];
cx q[6],q[13];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
u1(7.0685834705770345) q[14];
cx q[6],q[13];
u1(-0.7853981633974483) q[13];
u1(0.7853981633974483) q[6];
cx q[6],q[13];
u2(0.7853981633974483,3.141592653589793) q[13];
cx q[12],q[13];
u1(-0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
u3(3.141592653589793,0.0,3.141592653589793) q[13];
cx q[13],q[12];
u1(-0.7853981633974483) q[12];
cx q[11],q[12];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
u2(0.0,2.356194490192345) q[12];
cx q[13],q[14];
cx q[14],q[13];
cx q[13],q[14];
cx q[12],q[13];
u1(-0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
u1(-0.7853981633974483) q[12];
cx q[12],q[11];
cx q[11],q[12];
u2(0.0,3.9269908169872414) q[11];
cx q[10],q[11];
cx q[11],q[10];
cx q[10],q[11];
u1(0.7853981633974483) q[13];
cx q[12],q[13];
u1(0.7853981633974483) q[12];
u1(-0.7853981633974483) q[13];
cx q[12],q[13];
u2(0.7853981633974483,3.141592653589793) q[13];
cx q[14],q[13];
u1(-0.7853981633974483) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(0.7853981633974483) q[12];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[14],q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[17],q[11];
u1(-0.7853981633974483) q[11];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[23],q[17];
u1(0.7853981633974483) q[17];
cx q[11],q[17];
u2(0.0,2.356194490192345) q[17];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[11],q[17];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[17],q[11];
cx q[11],q[17];
cx q[17],q[23];
cx q[23],q[17];
cx q[17],q[23];
cx q[4],q[6];
cx q[6],q[4];
cx q[4],q[6];
cx q[13],q[6];
u1(-0.7853981633974483) q[6];
cx q[4],q[6];
u1(0.7853981633974483) q[6];
cx q[13],q[6];
u1(0.7853981633974483) q[13];
u1(-0.7853981633974483) q[6];
cx q[6],q[4];
cx q[4],q[6];
u2(0.0,3.9269908169872414) q[4];
cx q[6],q[13];
u1(-0.7853981633974483) q[13];
u1(0.7853981633974483) q[6];
cx q[6],q[13];
u2(0.7853981633974483,3.141592653589793) q[13];
cx q[13],q[12];
cx q[12],q[13];
cx q[13],q[12];
cx q[11],q[12];
u1(-0.7853981633974483) q[12];
cx q[11],q[12];
cx q[12],q[11];
cx q[11],q[12];
cx q[17],q[11];
u1(0.7853981633974483) q[11];
cx q[12],q[11];
u2(0.0,2.356194490192345) q[11];
