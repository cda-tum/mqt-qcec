OPENQASM 2.0;
include "qelib1.inc";
qreg q[16];
u3(1.5707963268013796, 3.698629342944752, -0.9721872384823051) q[0];
u3(1.3240512523628656, -4.150710758062049, 1.5707963268012235) q[1];
u3(3.1415926533569594, 2.510312365348886, 2.51031236533479) q[2];
u3(1.5707963267948966, 3.141592653589793, 3.141592653589793) q[3];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[4];
u3(1.5707963267948966, 3.141592653589793, 3.141592653589793) q[5];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[6];
u3(1.5707963267948966, 3.141592653589793, 3.141592653589793) q[7];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[8];
u3(1.5707963267948966, 3.141592653589793, 3.141592653589793) q[9];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[10];
u3(1.5707963267948966, 3.141592653589793, 3.141592653589793) q[11];
u3(1.5707963267948966, 2.220446049250313e-16, -3.141592653589793) q[12];
u3(1.5707963267948966, 3.141592653589793, -3.141592653589793) q[13];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[14];
u3(1.5707963267948966, 3.141592653589793, -3.141592653589793) q[15];
cx q[1], q[2];
u3(0.24999999999999845, -2.658849721384721, -2.1324745491152024) q[1];
u3(3.141592653296536, -1.365903695330139, -1.3659036953301318) q[2];
cx q[1], q[2];
u3(2.5093956798996886, 0.39052821881924293, -4.888365866816978) q[1];
u3(1.5707963269788476, 4.3391490400779276e-10, -3.1415926535753425) q[2];
cx q[0], q[1];
cx q[2], q[3];
u3(0.2500000000000003, -1.2380780130046798, -3.698629342945577) q[0];
u3(3.1354350518873244, -0.2674459248497576, 3.124072825983004) q[1];
u3(0.0, 0.0, 0.0) q[2];
u3(0.0, -0.125, -0.125) q[3];
cx q[0], q[1];
cx q[2], q[3];
u3(1.5707963268012426, 4.113779892072544, -1.9035146405836303) q[0];
u3(0.5990629440969317, -3.0974459246263017, 3.105125913167815) q[1];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[2];
u3(3.2868123855849206e-16, -0.7417268800995684, 0.7417268800995682) q[3];
cx q[1], q[2];
cx q[3], q[4];
u3(0.0, -0.125, -0.125) q[2];
u3(0.0, 0.0, 0.0) q[3];
u3(0.0, -0.125, -0.125) q[4];
cx q[1], q[2];
cx q[3], q[4];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[1];
u3(0.0, 0.0, 0.0) q[2];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[3];
u3(3.2868123855849206e-16, -0.7417268800995684, 0.7417268800995682) q[4];
cx q[0], q[1];
cx q[2], q[3];
u3(0.0, 0.0, 0.0) q[4];
u3(0.0, 0.0, 0.0) q[1];
u3(0.0, -0.125, -0.125) q[3];
cx q[4], q[5];
cx q[0], q[1];
cx q[2], q[3];
u3(0.0, 0.0, 0.0) q[4];
u3(0.0, -0.125, -0.125) q[5];
u3(1.5707963267950213, -0.6317295459958326, 1.5707963267944058) q[0];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[2];
u3(0.0, 0.0, 0.0) q[3];
cx q[4], q[5];
cx q[1], q[2];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[4];
u3(3.2868123855849206e-16, -0.7417268800995683, 0.7417268800995681) q[5];
u3(0.0, 0.0, 0.0) q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[1], q[2];
u3(0.0, 0.0, 0.0) q[3];
u3(0.0, -0.125, -0.125) q[4];
u3(0.0, 0.0, 0.0) q[5];
u3(0.0, -0.125, -0.125) q[6];
u3(1.8127572697279888, -1.57079632679488, 4.712388980384309) q[1];
cx q[3], q[4];
cx q[5], q[6];
cx q[0], q[1];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[3];
u3(6.123233995736766e-17, -3.0616169978683824e-17, -3.0616169978683824e-17) q[4];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[5];
u3(3.2868123855849206e-16, -0.7417268800995684, 0.7417268800995682) q[6];
u3(0.25000000000000044, -3.4855086130902952, 0.631729545995831) q[0];
u3(1.570796326794877, -1.1513394185562236, 3.341771304121721e-14) q[1];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(0.0, 0.0, 0.0) q[3];
u3(0.0, -0.125, -0.125) q[5];
u3(0.0, 0.0, 0.0) q[6];
u3(0.0, -0.125, -0.125) q[7];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(1.5707963215885818, 1.409820957414135, 3.1415926530450693) q[2];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[4];
u3(0.0, 0.0, 0.0) q[5];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[6];
u3(3.2868123855849206e-16, -0.7417268800995684, 0.7417268800995682) q[7];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(3.141592653589793, -2.0860896208594104, 1.2695127015734753) q[1];
u3(1.2696868602872497e-09, -0.16030889941082146, -0.08969110058929466) q[2];
u3(0.0, 0.0, 0.0) q[4];
u3(0.0, -0.125, -0.125) q[6];
u3(0.0, -0.125, -0.125) q[8];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(1.5385373536124667, -3.3895458235962157, -1.3060097758917473) q[1];
u3(3.1415926483112355, 3.0442350365316697, 1.3124633403560093) q[2];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[3];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[5];
u3(0.0, 0.0, 0.0) q[6];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[7];
u3(8.532843177179282e-17, 0.0, 0.0) q[8];
cx q[0], q[1];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(1.5707963267950198, 1.5707963267953868, 3.485508613090298) q[0];
u3(3.0108532527160725, 4.7123889803817445, 1.5707963267919416) q[1];
u3(0.0, -0.125, -0.125) q[3];
u3(0.0, 0.0, 0.0) q[5];
u3(0.0, -0.125, -0.125) q[7];
u3(0.0, 0.0, 0.0) q[8];
u3(0.0, -0.125, -0.125) q[9];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[2];
u3(8.532843177179282e-17, 0.0, 0.0) q[3];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[4];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[6];
u3(0.0, 0.0, 0.0) q[7];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[8];
u3(3.2868123855849206e-16, -0.7417268800995684, 0.7417268800995682) q[9];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(0.0, -0.125, -0.125) q[2];
u3(0.0, -0.125, -0.125) q[4];
u3(0.0, 0.0, 0.0) q[6];
u3(0.0, -0.125, -0.125) q[8];
u3(0.0, 0.0, 0.0) q[9];
u3(0.0, -0.125, -0.125) q[10];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[1];
u3(0.0, 0.0, 0.0) q[2];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[3];
u3(8.532843177179282e-17, 0.0, 0.0) q[4];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[5];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[7];
u3(0.0, 0.0, 0.0) q[8];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[9];
u3(3.2868123855849206e-16, -0.7417268800995684, 0.7417268800995682) q[10];
cx q[0], q[1];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(0.0, 0.0, 0.0) q[10];
u3(0.0, 0.0, 0.0) q[1];
u3(0.0, -0.125, -0.125) q[3];
u3(0.0, -0.125, -0.125) q[5];
u3(0.0, 0.0, 0.0) q[7];
u3(0.0, -0.125, -0.125) q[9];
cx q[10], q[11];
cx q[0], q[1];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(0.0, 0.0, 0.0) q[10];
u3(0.0, -0.125, -0.125) q[11];
u3(1.5707963270346463, 4.905536215506974, 0.9523732089951065) q[0];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[2];
u3(0.0, 0.0, 0.0) q[3];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[4];
u3(8.532843177179282e-17, 0.0, 0.0) q[5];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[6];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[8];
u3(0.0, 0.0, 0.0) q[9];
cx q[10], q[11];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[10];
u3(3.2868123855849206e-16, -0.7417268800995683, 0.7417268800995681) q[11];
u3(0.0, 0.0, 0.0) q[2];
u3(0.0, -0.125, -0.125) q[4];
u3(0.0, -0.125, -0.125) q[6];
u3(0.0, 0.0, 0.0) q[8];
cx q[9], q[10];
cx q[11], q[12];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(0.0, -0.125, -0.125) q[10];
u3(0.0, 0.0, 0.0) q[11];
u3(0.0, -0.125, -0.125) q[12];
u3(1.570796325242758, -2.458773818116612, 3.141592652536968) q[1];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[3];
u3(0.0, 0.0, 0.0) q[4];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[5];
u3(8.532843177179282e-17, 0.0, 0.0) q[6];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[7];
cx q[9], q[10];
cx q[11], q[12];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[9];
u3(0.0, 0.0, 0.0) q[10];
u3(1.5707963267948968, 0.0, 1.5707963267948968) q[11];
u3(3.2868123855849206e-16, -0.7417268800995683, 0.7417268800995681) q[12];
u3(0.0, 0.0, 0.0) q[3];
u3(0.0, -0.125, -0.125) q[5];
u3(0.0, -0.125, -0.125) q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(0.0, 0.0, 0.0) q[12];
cx q[2], q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(0.0, 0.0, 0.0) q[9];
u3(0.0, -0.125, -0.125) q[11];
cx q[12], q[13];
u3(1.5707963267948966, -2.4885287017487396, 3.141592653589793) q[2];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[4];
u3(0.0, 0.0, 0.0) q[5];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[6];
u3(8.532843177179282e-17, 0.0, 0.0) q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(0.0, 0.0, 0.0) q[12];
u3(0.0, -0.125, -0.125) q[13];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[8];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[10];
u3(0.0, 0.0, 0.0) q[11];
cx q[12], q[13];
u3(3.141592653589793, 1.2060252812431997, 1.5752346792533398) q[1];
u3(3.141592653589793, -1.018998629292777, 1.8725940242970547) q[2];
u3(0.0, 0.0, 0.0) q[4];
u3(0.0, -0.125, -0.125) q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(1.5707963267948968, 0.0, 1.5707963267948968) q[12];
u3(3.2868123855849206e-16, -0.7417268800995683, 0.7417268800995681) q[13];
cx q[1], q[2];
cx q[3], q[4];
cx q[5], q[6];
u3(0.0, -0.125, -0.125) q[8];
u3(0.0, 0.0, 0.0) q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(4.12280138366313, 6.084349441609635, 2.2751502149617964) q[1];
u3(-1.1816407439491567e-16, 3.390880583826311, 5.380833425112225) q[2];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[3];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[5];
u3(0.0, 0.0, 0.0) q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(0.0, -0.125, -0.125) q[14];
cx q[0], q[1];
cx q[2], q[3];
cx q[4], q[5];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[7];
u3(8.532843177179282e-17, 0.0, 0.0) q[8];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[9];
cx q[11], q[12];
cx q[13], q[14];
u3(0.24999999999997935, 5.637144911985536, 4.519241745169825) q[0];
u3(0.17481009696066513, 1.985413270930268, -1.7396173911305366) q[1];
u3(0.0, -0.125, -0.125) q[3];
u3(0.0, 0.0, 0.0) q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[11];
u3(0.0, 0.0, 0.0) q[12];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[13];
u3(3.2868123855849206e-16, -0.7417268800995683, 0.7417268800995681) q[14];
cx q[0], q[1];
cx q[2], q[3];
cx q[4], q[5];
u3(0.0, -0.125, -0.125) q[7];
u3(0.0, -0.125, -0.125) q[9];
cx q[10], q[11];
cx q[12], q[13];
u3(0.0, 0.0, 0.0) q[14];
u3(4.712388980150148, 5.330812098192436, 3.7876330488846137) q[0];
u3(-0.6281943376678667, 3.3439556855346355, 2.977090663618331) q[1];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[2];
u3(8.532843177179282e-17, 0.0, 0.0) q[3];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[4];
cx q[6], q[7];
cx q[8], q[9];
u3(0.0, 0.0, 0.0) q[11];
u3(0.0, -0.125, -0.125) q[13];
cx q[14], q[15];
cx q[1], q[2];
cx q[3], q[4];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[6];
u3(0.0, 0.0, 0.0) q[7];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[8];
u3(8.532843177179282e-17, 0.0, 0.0) q[9];
cx q[10], q[11];
cx q[12], q[13];
u3(0.0, 0.0, 0.0) q[14];
u3(0.0, -0.125, -0.125) q[15];
u3(0.0, -0.125, -0.125) q[2];
u3(0.0, -0.125, -0.125) q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[10];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[12];
u3(0.0, 0.0, 0.0) q[13];
cx q[14], q[15];
cx q[1], q[2];
cx q[3], q[4];
u3(0.0, 0.0, 0.0) q[6];
u3(0.0, -0.125, -0.125) q[8];
cx q[9], q[10];
cx q[11], q[12];
u3(1.5707963267948968, 0.0, 1.5707963267948968) q[14];
u3(1.001954908788063e-08, -0.6750953106187336, 5.387484244257511) q[15];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[1];
u3(0.0, 0.0, 0.0) q[2];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[3];
u3(8.532843177179282e-17, 0.0, 0.0) q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(0.0, -0.125, -0.125) q[10];
u3(0.0, 0.0, 0.0) q[12];
cx q[13], q[14];
cx q[0], q[1];
cx q[2], q[3];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[5];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[7];
u3(0.0, 0.0, 0.0) q[8];
cx q[9], q[10];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(0.0, -0.125, -0.125) q[14];
u3(0.0, 0.0, 0.0) q[1];
u3(0.0, -0.125, -0.125) q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[9];
u3(8.532843177179282e-17, 0.0, 0.0) q[10];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[11];
cx q[13], q[14];
cx q[0], q[1];
cx q[2], q[3];
u3(0.0, -0.125, -0.125) q[5];
u3(0.0, 0.0, 0.0) q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[13];
u3(1.2349848074883556, 0.5458175086869241, -4.040731003943112) q[14];
u3(1.5707963267949017, 3.376774268847033, -1.3012636636582287) q[0];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[2];
u3(0.0, 0.0, 0.0) q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(0.0, -0.125, -0.125) q[9];
u3(0.0, -0.125, -0.125) q[11];
cx q[12], q[13];
cx q[1], q[2];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[4];
u3(8.532843177179282e-17, 0.0, 0.0) q[5];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[6];
cx q[8], q[9];
cx q[10], q[11];
u3(0.0, 0.0, 0.0) q[13];
u3(0.0, 0.0, 0.0) q[2];
cx q[3], q[4];
cx q[5], q[6];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[8];
u3(0.0, 0.0, 0.0) q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[10];
u3(8.532843177179282e-17, 0.0, 0.0) q[11];
cx q[12], q[13];
cx q[1], q[2];
u3(0.0, -0.125, -0.125) q[4];
u3(0.0, -0.125, -0.125) q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[12];
u3(-0.34461501668122707, 2.2611057321048413, -2.3691113297768562) q[13];
u3(1.5707963267948946, -1.5288134904768458, -3.141592653589792) q[1];
cx q[3], q[4];
cx q[5], q[6];
u3(0.0, 0.0, 0.0) q[8];
u3(0.0, -0.125, -0.125) q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[3];
u3(0.0, 0.0, 0.0) q[4];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[5];
u3(8.532843177179282e-17, 0.0, 0.0) q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(3.141592653589793, 2.0126881177894114, 5.879517113613691) q[13];
u3(0.8633056785714065, 1.5707963268287566, -1.5707963267556666) q[14];
cx q[2], q[3];
cx q[4], q[5];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[7];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[9];
u3(0.0, 0.0, 0.0) q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(0.0, 0.0, 0.0) q[3];
u3(0.0, -0.125, -0.125) q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[11];
u3(8.532843177179282e-17, 0.0, 0.0) q[12];
u3(4.95676464593379, -0.24545667690278367, 0.6475226547767313) q[13];
u3(4.6666486860473775, 5.711974154042495, -0.94931678633701) q[14];
cx q[2], q[3];
cx q[4], q[5];
u3(0.0, -0.125, -0.125) q[7];
u3(0.0, 0.0, 0.0) q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(1.5707964966280614, -2.298938807345055, 3.14159251806691) q[2];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[4];
u3(0.0, 0.0, 0.0) q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(0.0, -0.125, -0.125) q[11];
u3(0.0, -0.125, -0.125) q[13];
u3(3.39159264971765, 7.250340276500068, 0.730362773605577) q[14];
u3(3.141592653524296, 2.5476369456609755, 5.689229597619214) q[15];
cx q[1], q[2];
cx q[3], q[4];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[6];
u3(8.532843177179282e-17, 0.0, 0.0) q[7];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[8];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(6.283185307179586, 5.98256536880236, 6.237343371549365) q[1];
u3(3.141592448435295, 4.976052187927914, 1.584459534336365) q[2];
u3(0.0, 0.0, 0.0) q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[10];
u3(0.0, 0.0, 0.0) q[11];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[12];
u3(8.532843177179282e-17, 0.0, 0.0) q[13];
u3(6.2831853139022416, 4.884794643671604, 3.5728283439785207) q[14];
u3(6.283185299907338, 0.7892067253236354, 7.064774956772292) q[15];
cx q[1], q[2];
cx q[3], q[4];
u3(0.0, -0.125, -0.125) q[6];
u3(0.0, -0.125, -0.125) q[8];
cx q[9], q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(0.0, 0.0, 0.0) q[15];
u3(4.9813321723799024, 6.301221304215631, 3.3782905271773154) q[1];
u3(3.1415926703140804, 3.118361062867952, 3.9610149091052045) q[2];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[3];
cx q[5], q[6];
cx q[7], q[8];
u3(0.0, 0.0, 0.0) q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(0.0, -0.125, -0.125) q[14];
cx q[0], q[1];
cx q[2], q[3];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[5];
u3(0.0, 0.0, 0.0) q[6];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[7];
u3(8.532843177179282e-17, 0.0, 0.0) q[8];
cx q[9], q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(0.24999999999999992, 1.044794366240841, 6.04800369192235) q[0];
u3(6.299379513583522, 3.2113345378857074, 3.3213300471071174) q[1];
u3(0.0, -0.125, -0.125) q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[9];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[11];
u3(0.0, 0.0, 0.0) q[12];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[13];
u3(8.532843177179282e-17, 0.0, 0.0) q[14];
cx q[0], q[1];
cx q[2], q[3];
u3(0.0, 0.0, 0.0) q[5];
u3(0.0, -0.125, -0.125) q[7];
cx q[8], q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(1.5707963267949012, 4.4428563172480215, 5.238390940938742) q[0];
u3(3.418539905419923, 3.3793585212795367, 3.370617726457057) q[1];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[2];
u3(8.532843177179282e-17, 0.0, 0.0) q[3];
cx q[4], q[5];
cx q[6], q[7];
u3(0.0, -0.125, -0.125) q[9];
u3(0.0, 0.0, 0.0) q[11];
u3(0.0, -0.125, -0.125) q[13];
u3(0.0, 0.0, 0.0) q[14];
u3(0.0, -0.125, -0.125) q[15];
cx q[1], q[2];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[4];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[6];
u3(0.0, 0.0, 0.0) q[7];
cx q[8], q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(0.0, -0.125, -0.125) q[2];
cx q[3], q[4];
cx q[5], q[6];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[8];
u3(8.532843177179282e-17, 0.0, 0.0) q[9];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[10];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[14];
u3(6.212470744432521e-09, -3.91904003601071, 2.3482437086679377) q[15];
cx q[1], q[2];
u3(0.0, -0.125, -0.125) q[4];
u3(0.0, 0.0, 0.0) q[6];
cx q[7], q[8];
cx q[9], q[10];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[1];
u3(0.0, 0.0, 0.0) q[2];
cx q[3], q[4];
cx q[5], q[6];
u3(0.0, -0.125, -0.125) q[8];
u3(0.0, -0.125, -0.125) q[10];
u3(0.0, 0.0, 0.0) q[12];
cx q[13], q[14];
cx q[0], q[1];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[3];
u3(8.532843177179282e-17, 0.0, 0.0) q[4];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[5];
cx q[7], q[8];
cx q[9], q[10];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(0.0, -0.125, -0.125) q[14];
u3(0.0, 0.0, 0.0) q[1];
cx q[2], q[3];
cx q[4], q[5];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[7];
u3(0.0, 0.0, 0.0) q[8];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[9];
u3(8.532843177179282e-17, 0.0, 0.0) q[10];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[11];
cx q[13], q[14];
cx q[0], q[1];
u3(0.0, -0.125, -0.125) q[3];
u3(0.0, -0.125, -0.125) q[5];
cx q[6], q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[13];
u3(1.9807995154418523, -2.078212939794722, 3.2956523905656794) q[14];
cx q[2], q[3];
cx q[4], q[5];
u3(0.0, 0.0, 0.0) q[7];
u3(0.0, -0.125, -0.125) q[9];
u3(0.0, -0.125, -0.125) q[11];
cx q[12], q[13];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[2];
u3(0.0, 0.0, 0.0) q[3];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[4];
u3(8.532843177179282e-17, 0.0, 0.0) q[5];
cx q[6], q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(0.0, 0.0, 0.0) q[13];
cx q[1], q[2];
cx q[3], q[4];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[6];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[8];
u3(0.0, 0.0, 0.0) q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[10];
u3(8.532843177179282e-17, 0.0, 0.0) q[11];
cx q[12], q[13];
u3(0.0, 0.0, 0.0) q[2];
u3(0.0, -0.125, -0.125) q[4];
cx q[5], q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[12];
u3(3.3879991213643272, 4.578193635810148, 9.503257427289979) q[13];
cx q[1], q[2];
cx q[3], q[4];
u3(0.0, -0.125, -0.125) q[6];
u3(0.0, 0.0, 0.0) q[8];
u3(0.0, -0.125, -0.125) q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[3];
u3(0.0, 0.0, 0.0) q[4];
cx q[5], q[6];
cx q[7], q[8];
cx q[9], q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(3.141592653589793, 3.0604454671441084, 4.144884159500745) q[13];
u3(1.297098473972984, -1.5707963268003862, 4.7123889803792505) q[14];
cx q[2], q[3];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[5];
u3(8.532843177179282e-17, 0.0, 0.0) q[6];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[7];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[9];
u3(0.0, 0.0, 0.0) q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(0.0, 0.0, 0.0) q[3];
cx q[4], q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[11];
u3(8.532843177179282e-17, 0.0, 0.0) q[12];
u3(1.3251639380407203, 6.302901462052073, 2.4401264184284965) q[13];
u3(7.3250471601395235, 1.9142345745043792, 5.098988495961082) q[14];
cx q[2], q[3];
u3(0.0, -0.125, -0.125) q[5];
u3(0.0, -0.125, -0.125) q[7];
u3(0.0, 0.0, 0.0) q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
cx q[4], q[5];
cx q[6], q[7];
cx q[8], q[9];
u3(0.0, -0.125, -0.125) q[11];
u3(0.0, -0.125, -0.125) q[13];
u3(0.2499999993454793, 4.536093485418275, 5.246360801466184) q[14];
u3(6.283185306699073, 3.269589702959485, 6.1551882580101624) q[15];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[4];
u3(0.0, 0.0, 0.0) q[5];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[6];
u3(8.532843177179282e-17, 0.0, 0.0) q[7];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[8];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[10];
u3(0.0, 0.0, 0.0) q[11];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[12];
u3(8.532843177179282e-17, 0.0, 0.0) q[13];
u3(3.141592657286629, 5.34321853880259, 0.8071250531182803) q[14];
u3(-6.0997096989931195e-09, 3.98219245075599, 0.7301965300386668) q[15];
u3(0.0, 0.0, 0.0) q[4];
u3(0.0, -0.125, -0.125) q[6];
u3(0.0, -0.125, -0.125) q[8];
cx q[9], q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(0.0, 0.0, 0.0) q[15];
cx q[3], q[4];
cx q[5], q[6];
cx q[7], q[8];
u3(0.0, 0.0, 0.0) q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(0.0, -0.125, -0.125) q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[5];
u3(0.0, 0.0, 0.0) q[6];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[7];
u3(8.532843177179282e-17, 0.0, 0.0) q[8];
cx q[9], q[10];
cx q[11], q[12];
cx q[13], q[14];
cx q[4], q[5];
cx q[6], q[7];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[9];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[11];
u3(0.0, 0.0, 0.0) q[12];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[13];
u3(8.532843177179282e-17, 0.0, 0.0) q[14];
u3(0.0, 0.0, 0.0) q[5];
u3(0.0, -0.125, -0.125) q[7];
cx q[8], q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
cx q[4], q[5];
cx q[6], q[7];
u3(0.0, -0.125, -0.125) q[9];
u3(0.0, 0.0, 0.0) q[11];
u3(0.0, -0.125, -0.125) q[13];
u3(0.0, 0.0, 0.0) q[14];
u3(0.0, -0.125, -0.125) q[15];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[6];
u3(0.0, 0.0, 0.0) q[7];
cx q[8], q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
cx q[5], q[6];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[8];
u3(8.532843177179282e-17, 0.0, 0.0) q[9];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[10];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[14];
u3(6.713171846466525e-10, 0.9063678041090947, 0.6644285226451768) q[15];
u3(0.0, 0.0, 0.0) q[6];
cx q[7], q[8];
cx q[9], q[10];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[14];
cx q[5], q[6];
u3(0.0, -0.125, -0.125) q[8];
u3(0.0, -0.125, -0.125) q[10];
u3(0.0, 0.0, 0.0) q[12];
cx q[13], q[14];
cx q[7], q[8];
cx q[9], q[10];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(0.0, -0.125, -0.125) q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[7];
u3(0.0, 0.0, 0.0) q[8];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[9];
u3(8.532843177179282e-17, 0.0, 0.0) q[10];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[11];
cx q[13], q[14];
cx q[6], q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[13];
u3(1.949938345728355, -2.026116675699649, -0.8307035065756702) q[14];
u3(0.0, 0.0, 0.0) q[7];
u3(0.0, -0.125, -0.125) q[9];
u3(0.0, -0.125, -0.125) q[11];
cx q[12], q[13];
cx q[6], q[7];
cx q[8], q[9];
cx q[10], q[11];
u3(0.0, 0.0, 0.0) q[13];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[8];
u3(0.0, 0.0, 0.0) q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[10];
u3(8.532843177179282e-17, 0.0, 0.0) q[11];
cx q[12], q[13];
cx q[7], q[8];
cx q[9], q[10];
u3(1.5707963267948966, 2.220446049250313e-16, 3.141592653589793) q[12];
u3(1.605335375884444, 4.457204268441797, -0.06292303420165449) q[13];
u3(0.0, 0.0, 0.0) q[8];
u3(0.0, -0.125, -0.125) q[10];
cx q[11], q[12];
cx q[13], q[14];
cx q[7], q[8];
cx q[9], q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(6.283185307179586, 4.062345583219204, 4.598680286841274) q[13];
u3(3.1415926535810836, 5.420189709808817, 5.420189709808819) q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[9];
u3(0.0, 0.0, 0.0) q[10];
cx q[11], q[12];
cx q[13], q[14];
cx q[8], q[9];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[11];
u3(8.532843177179282e-17, 0.0, 0.0) q[12];
u3(3.2133608860518774, 5.214840837722617, 4.661894196053515) q[13];
u3(0.8937862065670165, 1.2440266896429848, 2.3603106410687906) q[14];
u3(0.0, 0.0, 0.0) q[9];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
cx q[8], q[9];
u3(0.0, -0.125, -0.125) q[11];
u3(0.0, -0.125, -0.125) q[13];
u3(0.24999999999968553, 5.611891912011103, 5.53395522837392) q[14];
u3(3.1415926535902328, 6.16128022336242, 3.0196875697726195) q[15];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[10];
u3(0.0, 0.0, 0.0) q[11];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[12];
u3(8.532843177179282e-17, 0.0, 0.0) q[13];
u3(6.2831853063337215, 0.5653734675032248, 6.389105234677195) q[14];
u3(3.1415926529149787, 5.603644741575092, 4.032848414739265) q[15];
cx q[9], q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(0.0, 0.0, 0.0) q[15];
u3(0.0, 0.0, 0.0) q[10];
u3(0.0, -0.125, -0.125) q[12];
u3(0.0, -0.125, -0.125) q[14];
cx q[9], q[10];
cx q[11], q[12];
cx q[13], q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[11];
u3(0.0, 0.0, 0.0) q[12];
u3(1.5707963267948966, 1.1102230246251565e-16, 1.570796326794897) q[13];
u3(8.532843177179282e-17, 0.0, 0.0) q[14];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(0.0, 0.0, 0.0) q[11];
u3(0.0, -0.125, -0.125) q[13];
u3(0.0, 0.0, 0.0) q[14];
u3(0.0, -0.125, -0.125) q[15];
cx q[10], q[11];
cx q[12], q[13];
cx q[14], q[15];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(1.5707963267948968, 0.0, -4.71238898038469) q[14];
u3(2.1965156326459443, -2.8222790205050905, -2.627670694261884) q[15];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[14];
u3(0.0, 0.0, 0.0) q[12];
cx q[13], q[14];
cx q[11], q[12];
u3(0.0, 0.0, 0.0) q[13];
u3(0.0, -0.125, -0.125) q[14];
cx q[13], q[14];
u3(1.5707963267948966, -1.5707963267948966, 1.5707963267948966) q[13];
u3(1.0277339916976553, 0.14659123773901017, -1.4327029342332407) q[14];
cx q[12], q[13];
u3(0.0, 0.0, 0.0) q[13];
cx q[12], q[13];
u3(2.6795523880654355, 4.953912199856304, 13.708133806449572) q[13];
cx q[13], q[14];
u3(3.141592653589793, 5.965349845280416, 9.198382981006143) q[13];
u3(6.283185314874866, 0.36918853068220286, 2.7724041170758302) q[14];
cx q[13], q[14];
u3(0.4620402655243577, 5.141422114821867, 5.0453526822314965) q[13];
u3(5.25545131442516, 6.274253533786785, 0.14659119102044613) q[14];
cx q[14], q[15];
u3(-1.5970406745757394e-09, 4.735814519429113, 1.8289194382804266) q[14];
u3(3.4305321712208463, 2.492071817710516, -0.7714176214060169) q[15];
cx q[14], q[15];
u3(4.7123890176797145, 4.712389005059283, 3.007069173802136) q[14];
u3(4.0238360530750805, 2.3224104472296476, 2.5445026374178767) q[15];
