import json
import os
import sys
import argparse

if __name__ == '__main__':
    # set paths for all files
    filepath = '../../Benchmarks/EquivalenceCheckingEvaluation/revlib/'
    methods = ['Reference', 'Naive', 'Proportional', 'Lookahead', 'CompilationFlow', 'PowerOfSimulation']
    benchmarks = ['o0', 'o1', 'o2', 'o3']

    schedulepaths = {}
    schedules = {}
    for method in methods:
        schedulepaths[method] = './schedule_' + method
        schedules[method] = {}

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

    for filename, d in data.items():
        for benchmark in benchmarks:
            for method, results in d[benchmark].items():
                if method in methods:
                    if d[benchmark][method]['time'] is None:
                        schedule = schedules[method]
                        if filename not in schedule:
                            schedule[filename] = {}
                        if benchmark not in schedule[filename]:
                            schedule[filename][benchmark] = []
                        if method not in schedule[filename][benchmark]:
                            schedule[filename][benchmark].append(method)

    if not args.exclude_all_timeout:
        for f in os.listdir(filepath):
            if os.path.isfile(os.path.join(filepath, f)) and not f.startswith('.'):
                name = os.path.splitext(f)[0]
                for method in methods:
                    schedule = schedules[method]
                    if name not in schedule:
                        schedule[name] = {}
                        for benchmark in benchmarks:
                            schedule[name][benchmark] = [method]

    for method in methods:
        schedule = schedules[method]
        schedulepath = schedulepaths[method]
        with open(schedulepath, 'w') as schedulefile:
            schedulefile.write(json.dumps(schedule, indent=4))
