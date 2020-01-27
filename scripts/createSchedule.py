import json
import os
import sys
import argparse

if __name__ == '__main__':
    # set paths for all files
    revlibPath = '../../Benchmarks/EquivalenceCheckingEvaluation/revlib/'
    scheduleFilePath_ref = './schedule_ref.json'
    scheduleFilePath_flow = './schedule_flow.json'

    parser = argparse.ArgumentParser(description='Create schedule for running computations.')
    parser.add_argument("--exclude_all_timeout", action='store_true')
    parser.add_argument("--existing_data")
    args = parser.parse_args()

    if args.existing_data:
        # read in existing data
        if os.path.exists(args.existing_data):
            with open(args.existing_data) as jsonFile:
                data = json.load(jsonFile)
        else:
            sys.exit()
    else:
        data = {}

    schedule_ref = {}
    schedule_flow = {}
    for filename, d in data.items():
        for benchmark in ['o0', 'o1', 'o2', 'o3']:
            for method, results in d[benchmark].items():
                if method in ['Reference', 'CompilationFlow']:
                    if d[benchmark][method]['time'] is None:
                        if method in ['Reference']:
                            if filename not in schedule_ref:
                                schedule_ref[filename] = {}
                            if benchmark not in schedule_ref[filename]:
                                schedule_ref[filename][benchmark] = []
                            if method not in schedule_ref[filename][benchmark]:
                                schedule_ref[filename][benchmark].append(method)
                        else:
                            if filename not in schedule_flow:
                                schedule_flow[filename] = {}
                            if benchmark not in schedule_flow[filename]:
                                schedule_flow[filename][benchmark] = []
                            if method not in schedule_flow[filename][benchmark]:
                                schedule_flow[filename][benchmark].append(method)

    if not args.exclude_all_timeout:
        for f in os.listdir(revlibPath):
            if os.path.isfile(os.path.join(revlibPath, f)) and not f.startswith('.'):
                name = os.path.splitext(f)[0]
                if name not in schedule_ref:
                    schedule_ref[name] = {}
                    for benchmark in ['o0', 'o1', 'o2', 'o3']:
                        schedule_ref[name][benchmark] = ['Reference']
                if name not in schedule_flow:
                    schedule_flow[name] = {}
                    for benchmark in ['o0', 'o1', 'o2', 'o3']:
                        schedule_flow[name][benchmark] = ['CompilationFlow']

    with open(scheduleFilePath_ref, 'w') as scheduleFile_ref:
        scheduleFile_ref.write(json.dumps(schedule_ref, indent=4))
    scheduleFile_ref.close()

    with open(scheduleFilePath_flow, 'w') as scheduleFile_flow:
        scheduleFile_flow.write(json.dumps(schedule_flow, indent=4))
    scheduleFile_flow.close()
