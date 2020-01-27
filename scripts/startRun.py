import json
import os
import sys
import subprocess
from datetime import datetime
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Execute given schedule.')
    parser.add_argument('--schedule')
    parser.add_argument('--timeout', nargs='?', const='10m', default='10m')
    parser.add_argument('--optimization_level', nargs='?', const=1, type=int, default=1)
    args = parser.parse_args()
    optimization_level = 'o' + str(args.optimization_level)

    # set paths for all files
    csvFilePath = '../results/results_' + optimization_level + '_' + datetime.now().strftime("%m_%d_%H_%M_%S") + '.csv'
    scheduleFilePath = args.schedule
    revlibPath = '../../Benchmarks/EquivalenceCheckingEvaluation/revlib/'
    transpiledPath = '../../Benchmarks/EquivalenceCheckingEvaluation/qasm_' + optimization_level + '/transpiled/'

    # read in schedule
    if os.path.exists(scheduleFilePath):
        with open(scheduleFilePath) as scheduleFile:
            schedule = json.load(scheduleFile)
    elif os.path.exists('../scripts/' + scheduleFilePath):
        with open('../scripts/' + scheduleFilePath) as scheduleFile:
            schedule = json.load(scheduleFile)
    else:
        sys.exit()

    with open(csvFilePath, 'a') as csvFile:
        for benchmark, runs in schedule.items():
            if optimization_level in runs:
                for method in runs[optimization_level]:
                    c = subprocess.Popen(['timeout',
                                          args.timeout,
                                          './QCEC_app',
                                          revlibPath + benchmark + '.real',
                                          transpiledPath + benchmark + '_transpiled.qasm',
                                          method,
                                         '--augment_qubits',
                                          '--print_csv'],
                                         stdout=csvFile,
                                         stderr=csvFile,
                                         universal_newlines=True)
                    c.wait()
                    csvFile.flush()

    csvFile.close()
