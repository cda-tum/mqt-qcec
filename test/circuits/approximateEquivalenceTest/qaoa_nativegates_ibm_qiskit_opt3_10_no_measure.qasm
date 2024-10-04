// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2
// Used Gate Set: ['id', 'rz', 'sx', 'x', 'cx', 'measure', 'barrier']

OPENQASM 2.0;
include "qelib1.inc";
qreg q[10];
creg meas[10];
rz(pi/2) q[0];
sx q[0];
rz(pi) q[0];
rz(pi/2) q[1];
sx q[1];
rz(pi/2) q[1];
cx q[0],q[1];
rz(1.289442989282985) q[1];
cx q[0],q[1];
rz(pi/2) q[2];
sx q[2];
rz(pi) q[2];
rz(pi/2) q[3];
sx q[3];
rz(pi/2) q[3];
cx q[1],q[3];
rz(1.289442989282985) q[3];
cx q[1],q[3];
rz(pi/2) q[1];
sx q[1];
rz(13.2110965090601) q[1];
sx q[1];
rz(5*pi/2) q[1];
rz(pi/2) q[4];
sx q[4];
rz(pi/2) q[4];
cx q[3],q[4];
rz(1.289442989282985) q[4];
cx q[3],q[4];
rz(pi/2) q[3];
sx q[3];
rz(13.2110965090601) q[3];
sx q[3];
rz(5*pi/2) q[3];
rz(pi/2) q[5];
sx q[5];
rz(pi) q[5];
rz(pi/2) q[6];
sx q[6];
rz(pi/2) q[6];
cx q[0],q[6];
rz(1.289442989282985) q[6];
cx q[0],q[6];
sx q[0];
rz(13.2110965090601) q[0];
sx q[0];
rz(3*pi) q[0];
cx q[0],q[1];
rz(-0.6447220641786215) q[1];
cx q[0],q[1];
cx q[1],q[3];
cx q[2],q[6];
rz(-0.6447220641786215) q[3];
cx q[1],q[3];
rz(pi/2) q[1];
sx q[1];
rz(17.56010762207137) q[1];
sx q[1];
rz(5*pi/2) q[1];
rz(1.289442989282985) q[6];
cx q[2],q[6];
rz(pi/2) q[6];
sx q[6];
rz(13.2110965090601) q[6];
sx q[6];
rz(5*pi/2) q[6];
cx q[0],q[6];
rz(-0.6447220641786215) q[6];
cx q[0],q[6];
sx q[0];
rz(17.56010762207137) q[0];
sx q[0];
rz(5*pi/2) q[0];
rz(pi/2) q[7];
sx q[7];
rz(pi/2) q[7];
cx q[5],q[7];
rz(1.289442989282985) q[7];
cx q[5],q[7];
rz(pi/2) q[8];
sx q[8];
rz(pi/2) q[8];
cx q[2],q[8];
rz(1.289442989282985) q[8];
cx q[2],q[8];
sx q[2];
rz(13.2110965090601) q[2];
sx q[2];
rz(3*pi) q[2];
cx q[2],q[6];
cx q[4],q[8];
rz(-0.6447220641786215) q[6];
cx q[2],q[6];
rz(pi/2) q[6];
sx q[6];
rz(17.56010762207137) q[6];
sx q[6];
rz(5*pi/2) q[6];
rz(1.289442989282985) q[8];
cx q[4],q[8];
rz(pi/2) q[4];
sx q[4];
rz(13.2110965090601) q[4];
sx q[4];
rz(5*pi/2) q[4];
cx q[3],q[4];
rz(-0.6447220641786215) q[4];
cx q[3],q[4];
rz(pi/2) q[3];
sx q[3];
rz(17.56010762207137) q[3];
sx q[3];
rz(5*pi/2) q[3];
rz(pi/2) q[8];
sx q[8];
rz(13.2110965090601) q[8];
sx q[8];
rz(5*pi/2) q[8];
cx q[2],q[8];
rz(-0.6447220641786215) q[8];
cx q[2],q[8];
sx q[2];
rz(17.56010762207137) q[2];
sx q[2];
rz(5*pi/2) q[2];
cx q[4],q[8];
rz(-0.6447220641786215) q[8];
cx q[4],q[8];
rz(pi/2) q[4];
sx q[4];
rz(17.56010762207137) q[4];
sx q[4];
rz(5*pi/2) q[4];
rz(pi/2) q[8];
sx q[8];
rz(17.56010762207137) q[8];
sx q[8];
rz(5*pi/2) q[8];
rz(pi/2) q[9];
sx q[9];
rz(pi/2) q[9];
cx q[5],q[9];
rz(1.289442989282985) q[9];
cx q[5],q[9];
sx q[5];
rz(13.2110965090601) q[5];
sx q[5];
rz(3*pi) q[5];
cx q[7],q[9];
rz(1.289442989282985) q[9];
cx q[7],q[9];
rz(pi/2) q[7];
sx q[7];
rz(13.2110965090601) q[7];
sx q[7];
rz(5*pi/2) q[7];
cx q[5],q[7];
rz(-0.6447220641786215) q[7];
cx q[5],q[7];
rz(pi/2) q[9];
sx q[9];
rz(13.2110965090601) q[9];
sx q[9];
rz(5*pi/2) q[9];
cx q[5],q[9];
rz(-0.6447220641786215) q[9];
cx q[5],q[9];
sx q[5];
rz(17.56010762207137) q[5];
sx q[5];
rz(5*pi/2) q[5];
cx q[7],q[9];
rz(-0.6447220641786215) q[9];
cx q[7],q[9];
rz(pi/2) q[7];
sx q[7];
rz(17.56010762207137) q[7];
sx q[7];
rz(5*pi/2) q[7];
rz(pi/2) q[9];
sx q[9];
rz(17.56010762207137) q[9];
sx q[9];
rz(5*pi/2) q[9];
