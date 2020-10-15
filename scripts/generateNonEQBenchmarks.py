import os
import subprocess
from pathlib import Path
import argparse

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Generate non-equivalent benchmarks by randomly removing gates.')
    parser.add_argument('--benchmarkdir', nargs='?', const='../../Benchmarks/EquivalenceCheckingEvaluation', default='../../Benchmarks/EquivalenceCheckingEvaluation')
    parser.add_argument('--optimization_level', nargs='?', const=1, type=int, default=1)
    parser.add_argument('--gates_to_remove', nargs='?', const=3, type=int, default=3)
    parser.add_argument('--ninstances', nargs='?', const=10, type=int, default=10)

    args = parser.parse_args()
    optimization_level = 'o' + str(args.optimization_level)
    benchmarkdir = args.benchmarkdir
    gates_to_remove = args.gates_to_remove
    ninstances = args.ninstances

    inputdir = benchmarkdir + '/qasm_' + optimization_level + '/transpiled/'
    outputbasedir = benchmarkdir + '/qasm_' + optimization_level + '/remove_' + str(gates_to_remove) + '/'

    Path(outputbasedir).mkdir(parents=True, exist_ok=True)

    for i in range(ninstances):
        outputdir = outputbasedir + str(i) + '/'
        Path(outputdir).mkdir(parents=True, exist_ok=True)
        with os.scandir(inputdir) as entries:
            for entry in entries:
                inputfile = inputdir + entry.name
                outputfile = outputdir + os.path.splitext(entry.name)[0] + '_removed_' + str(gates_to_remove) + '_' + str(i) + '.qasm'
                c = subprocess.Popen(['qfr_app',
                                      inputfile,
                                      outputfile,
                                      '--remove_gates',
                                      str(gates_to_remove)
                                      ],
                                     universal_newlines=True)
                c.wait()


