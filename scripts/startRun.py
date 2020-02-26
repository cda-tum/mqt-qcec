import json
import os
import sys
import subprocess
from datetime import datetime
import argparse
import logging

if __name__ == '__main__':

    methods = ['Reference', 'Naive', 'Proportional', 'Lookahead', 'CompilationFlow', 'PowerOfSimulation', 'None']

    parser = argparse.ArgumentParser(description='Execute given schedule.')
    parser.add_argument('--schedule')
    parser.add_argument('--timeout', nargs='?', const='10m', default='10m')
    parser.add_argument('--optimization_level', nargs='?', const=1, type=int, default=1)
    parser.add_argument('--method', default='None', const='None', nargs='?', choices=methods)
    parser.add_argument('--benchmarkdir', nargs='?', const='../../Benchmarks/EquivalenceCheckingEvaluation', default='../../Benchmarks/EquivalenceCheckingEvaluation')
    parser.add_argument('--basedir', nargs='?', const='..', default='..')
    args = parser.parse_args()
    optimization_level = 'o' + str(args.optimization_level)
    benchmarkdir = args.benchmarkdir
    basedir = args.basedir

    m = args.method
    if m == 'None':
        m = ''
    else:
        m = m + '_'
    # set paths for all files
    csvFilePath = basedir + '/results/results_' + m + optimization_level + '_' + datetime.now().strftime("%m_%d_%H_%M_%S") + '.csv'
    scheduleFilePath = args.schedule
    revlibPath = benchmarkdir + '/revlib/'
    transpiledPath = benchmarkdir + '/qasm_' + optimization_level + '/transpiled/'

    # read in schedule
    if os.path.exists(scheduleFilePath):
        with open(scheduleFilePath) as scheduleFile:
            schedule = json.load(scheduleFile)
    elif os.path.exists(basedir + '/scripts/' + scheduleFilePath):
        with open(basedir + '/scripts/' + scheduleFilePath) as scheduleFile:
            schedule = json.load(scheduleFile)
    else:
        sys.exit()

    logging.basicConfig(filename=basedir + '/results/log.txt', level=logging.INFO, format='[%(levelname)s] [%(asctime)s] %(message)s', datefmt='%d/%m/%Y %H:%M:%S')
    logging.info('Starting runs for optimization level: %s and schedule: %s', optimization_level, scheduleFilePath)

    counter = 0
    with open(csvFilePath, 'a') as csvFile:
        for benchmark, runs in schedule.items():
            if optimization_level in runs:
                for method in runs[optimization_level]:
                    logging.info("[%d] Starting run '%s' with '%s' method", counter, benchmark, method)
                    c = subprocess.Popen(['timeout',
                                          args.timeout,
                                          './QCEC_app',
                                          revlibPath + benchmark + '.real',
                                          transpiledPath + benchmark + '_transpiled.qasm',
                                          method,
                                         '--augment_qubits',
                                          '--print_csv'],
                                         stdout=csvFile,
                                         stderr=subprocess.PIPE,
                                         universal_newlines=True)
                    c.wait()
                    _, errors = c.communicate()
                    if errors:
                        logging.error(errors)
                    logging.info("[%d] Finished run '%s' with '%s' method", counter, benchmark, method)
                    csvFile.flush()
                    counter = counter + 1
