import csv
import json
import os
from json2html import *
import argparse

if __name__ == '__main__':
    methods = ['Reference', 'Naive', 'Proportional', 'Lookahead', 'CompilationFlow', 'PowerOfSimulation']
    opt_levels = ['o0', 'o1', 'o2', 'o3']
    parser = argparse.ArgumentParser(description='Incorporate results.')
    parser.add_argument('--results_file')
    parser.add_argument('--optimization_level', nargs='?', const=1, type=int, default=1)
    args = parser.parse_args()
    optimization_level = 'o' + str(args.optimization_level)

    # set paths for all files
    csvFilePath = args.results_file
    jsonFilePath = '../results/results_evaluation.json'
    htmlFilePath = '../results/results_evaluation.html'
    allResultsCSVPath = '../results/all_results_evaluation.csv'
    # set header for csv entries
    fieldnames = 'filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive;nsims'.split(";")

    # read in existing data
    if os.path.exists(jsonFilePath):
        with open(jsonFilePath) as jsonFile:
            data = json.load(jsonFile)
    else:
        data = {}

    # iterate over csv file
    with open(csvFilePath) as csvFile:
        csvReader = csv.DictReader(csvFile, delimiter=';', fieldnames=fieldnames)
        for rows in csvReader:
            # determine benchmark name
            filename = os.path.splitext(os.path.basename(rows['filename1']))[0]

            # create dictionaries if not already existing
            if filename not in data:
                data[filename] = {}
                data[filename]['original_circuit'] = {'qubits': None, 'gates': None}
                for level in opt_levels:
                    data[filename][level] = {'qubits': None, 'gates': None}
                    for method in methods:
                        data[filename][level][method] = {'time': None, 'maxActive': None}
                        if method is 'PowerOfSimulation':
                            data[filename][level][method]["nsims"] = None

            if rows['method'] not in methods:
                continue

            # populate dictionaries
            data[filename]['original_circuit'] = {'qubits': rows['nqubits1'], 'gates': rows['ngates1']}
            data[filename][optimization_level]['qubits'] = rows['nqubits2']
            data[filename][optimization_level]['gates'] = rows['ngates2']
            data[filename][optimization_level][rows['method']] = {'time': rows['time'], 'maxActive': rows['maxActive']}
            if rows['method'] is 'PowerOfSimulation':
                data[filename][optimization_level][rows['method']]['nsims'] = rows['nsims']

    # output json file
    with open(jsonFilePath, 'w') as jsonFile:
        jsonFile.write(json.dumps(data, indent=4))
    jsonFile.close()

    # output html nested tables
    with open(htmlFilePath, 'w') as htmlFile:
        htmlFile.write(json2html.convert(json=data))
    htmlFile.close()

    # generate csv for all results
    with open(allResultsCSVPath, 'w') as all_csv_results:
        header = ['name',
                  'q_orig', 'g_orig']
        for opt_level in opt_levels:
            header.append('q_' + opt_level)
            header.append('g_' + opt_level)
            for method in methods:
                header.append('t_' + opt_level + '_' + method)
                header.append('max_' + opt_level + '_' + method)
                if method is 'PowerOfSimulation':
                    header.append('nsims_' + opt_level)

        csvWriter = csv.DictWriter(all_csv_results, delimiter=';', fieldnames=header)
        csvWriter.writeheader()
        for name in data:
            elem = data[name]
            row = {'name': name,
                   'q_orig': elem['original_circuit']['qubits'],
                   'g_orig': elem['original_circuit']['gates']}
            for opt_level in opt_levels:
                opt_elem = elem[opt_level]
                row['q_'+opt_level] = opt_elem['qubits']
                row['g_'+opt_level] = opt_elem['gates']
                for method in methods:
                    m_opt_elem = opt_elem[method]
                    row['t_' + opt_level + '_' + method] = m_opt_elem['time']
                    row['max_' + opt_level + '_' + method] = m_opt_elem['maxActive']
                    if method is 'PowerOfSimulation':
                        row['nsims_' + opt_level] = m_opt_elem['nsims']

            csvWriter.writerow(row)
