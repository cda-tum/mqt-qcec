OPENQASM 2.0;
include "qelib1.inc";
qreg q[10];
u3(3.003909754504298, 0.9755289691212459, 1.0509096344814997) q[0];
u3(4.156718440227518, 2.611643357503636, 2.08567806863232) q[1];
u3(4.779639710281416, 0.3948887108584252, 4.639866888915957) q[2];
u3(4.913529737153367, 1.8022278630260664, 1.1500829634420369) q[3];
u3(7.36959198493212, 1.5870727469619421, 3.705345729525691) q[4];
u3(4.5213194521292905, 3.8962985426187635, 0.9264043971866853) q[5];
u3(1.922121831592675, 1.2827202706029506, 4.264037426734242) q[6];
u3(4.943093791569544, 0.10303076460489785, 0.8188385388339069) q[7];
u3(4.336041653615549, 1.3324893286344945, 6.343377074933315) q[8];
u3(-1.7838511809976763e-10, 4.203743296059084, 0.5086456834278801) q[9];
u3(2.4163853189395956, 6.465387066236423, 0.2090068671218588) q[0];
u3(0.21607052563769302, 4.224862904871233, 3.633251715427373) q[1];
u3(3.644050523995374, 6.479050866837511, 1.873099425712036) q[2];
u3(0.6633601445653122, 0.1950915333412193, 5.890474757097994) q[4];
u3(5.290832759626098, 2.644757609848364, 3.9110307486611813) q[5];
u3(-0.48815664772476397, 1.448663406745269, 4.7705392187328) q[6];
u3(2.3487512761759, 2.8040036725094883, 4.35845578829468) q[7];
u3(5.583800835000085, 4.696957583744842, 1.8900735246008296) q[8];
u3(1.2737849195603657, 5.609243524170237, 1.613725465604109) q[1];
u3(4.808378709201391, 3.8771753180928465, -0.3824185919056889) q[2];
cx q[3], q[4];
u3(5.338606814057927, 3.1058094049256875, 2.250621063711204) q[5];
u3(7.962807285656218, 3.488001019075014, 5.038865511119175) q[6];
cx q[8], q[9];
u3(3.4419025763850546, 6.143227142050376, 0.5274818522890575) q[1];
u3(1.951220248929252, 3.2445591686879554, 0.872719601458409) q[2];
u3(6.283120951089399, 3.072256825969127, 4.130006565382937) q[3];
u3(-0.0004781938832750595, 0.7107157064457341, 2.4317932982022046) q[4];
u3(2.251864506562823, 3.9974892422446393, 2.234968321661341) q[5];
u3(2.798485571380734, 2.813553652975767, 4.056102186411141) q[6];
u3(3.1441195882424173, 0.356473456783513, 1.8618018838221935) q[8];
u3(3.1415926535785372, 1.7946677720108122, 0.22820913361587367) q[9];
u3(1.8936855133449952, -0.6477761474203892, 2.5895021398664824) q[3];
cx q[7], q[8];
u3(7.333270418993937, 3.515795153397013, 5.167535516857539) q[3];
u3(6.280951615105036, 2.948394915703154, 2.0937973302206854) q[7];
u3(6.282241702901769, 2.5361182373574205, -0.962542980648663) q[8];
u3(1.5233405245621463, 0.44332681832304566, 2.641077451127197) q[7];
u3(2.070412344929058, 3.1947281124932627, 1.1533011486972722) q[7];