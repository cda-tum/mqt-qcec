import csv
import json
import os
from json2html import *
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Incorporate results.')
    parser.add_argument('--results_file')
    parser.add_argument('--optimization_level', nargs='?', const=1, type=int, default=1)
    args = parser.parse_args()
    optimization_level = 'o' + str(args.optimization_level)

    # set paths for all files
    csvFilePath = args.results_file
    jsonFilePath = '../results/results_compilationflow.json'
    htmlFilePath = '../results/results_compilationflow.html'
    allResultsCSVPath = '../results/all_results_compilationflow.csv'
    # set header for csv entries
    fieldnames = 'filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive'.split(";")

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
                data[filename]['o0'] = {'qubits': None, 'gates': None, 
                                                           'Reference': {'time': None, 'maxActive': None}, 
                                                           'CompilationFlow': {'time': None, 'maxActive': None}}
                data[filename]['o1'] = {'qubits': None, 'gates': None,
                                                           'Reference': {'time': None, 'maxActive': None},
                                                           'CompilationFlow': {'time': None, 'maxActive': None}}
                data[filename]['o2'] = {'qubits': None, 'gates': None,
                                                           'Reference': {'time': None, 'maxActive': None},
                                                           'CompilationFlow': {'time': None, 'maxActive': None}}
                data[filename]['o3'] = {'qubits': None, 'gates': None,
                                                           'Reference': {'time': None, 'maxActive': None},
                                                           'CompilationFlow': {'time': None, 'maxActive': None}}

            if rows['method'] not in ['Reference', 'CompilationFlow']:
                continue

            # populate dictionaries
            data[filename]['original_circuit'] = {'qubits': rows['nqubits1'], 'gates': rows['ngates1']}
            data[filename][optimization_level]['qubits'] = rows['nqubits2']
            data[filename][optimization_level]['gates'] = rows['ngates2']
            data[filename][optimization_level][rows['method']] = {'time': rows['time'], 'maxActive': rows['maxActive']}

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
                  'q_orig', 'g_orig',
                  'q_o0', 'g_o0', 't_o0_ref', 'max_o0_ref', 't_o0_new', 'max_o0_new',
                  'q_o1', 'g_o1', 't_o1_ref', 'max_o1_ref', 't_o1_new', 'max_o1_new',
                  'q_o2', 'g_o2', 't_o2_ref', 'max_o2_ref', 't_o2_new', 'max_o2_new',
                  'q_o3', 'g_o3', 't_o3_ref', 'max_o3_ref', 't_o3_new', 'max_o3_new']
        csvWriter = csv.DictWriter(all_csv_results, delimiter=';', fieldnames=header)
        csvWriter.writeheader()
        for name in data:
            csvWriter.writerow({'name': name,
                                'q_orig': data[name]['original_circuit']['qubits'], 
                                'g_orig': data[name]['original_circuit']['gates'],
                                'q_o0': data[name]['o0']['qubits'], 
                                'g_o0': data[name]['o0']['gates'],
                                't_o0_ref': data[name]['o0']['Reference']['time'], 
                                'max_o0_ref': data[name]['o0']['Reference']['maxActive'],
                                't_o0_new': data[name]['o0']['CompilationFlow']['time'],
                                'max_o0_new': data[name]['o0']['CompilationFlow']['maxActive'],

                                'q_o1': data[name]['o1']['qubits'],
                                'g_o1': data[name]['o1']['gates'],
                                't_o1_ref': data[name]['o1']['Reference']['time'],
                                'max_o1_ref': data[name]['o1']['Reference']['maxActive'],
                                't_o1_new': data[name]['o1']['CompilationFlow']['time'],
                                'max_o1_new': data[name]['o1']['CompilationFlow']['maxActive'],

                                'q_o2': data[name]['o2']['qubits'],
                                'g_o2': data[name]['o2']['gates'],
                                't_o2_ref': data[name]['o2']['Reference']['time'],
                                'max_o2_ref': data[name]['o2']['Reference']['maxActive'],
                                't_o2_new': data[name]['o2']['CompilationFlow']['time'],
                                'max_o2_new': data[name]['o2']['CompilationFlow']['maxActive'],

                                'q_o3': data[name]['o3']['qubits'],
                                'g_o3': data[name]['o3']['gates'],
                                't_o3_ref': data[name]['o3']['Reference']['time'],
                                'max_o3_ref': data[name]['o3']['Reference']['maxActive'],
                                't_o3_new': data[name]['o3']['CompilationFlow']['time'],
                                'max_o3_new': data[name]['o3']['CompilationFlow']['maxActive']})
