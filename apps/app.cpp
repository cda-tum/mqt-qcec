/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <memory>
#include <string>
#include <cctype>
#include <algorithm>

#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"

void show_usage(const std::string& name) {
	std::cerr << "Usage: " << name << "<PATH_TO_FILE_1> <PATH_TO_FILE_2> (<method>) (--print_csv)" << std::endl;
	std::cerr << "Available methods:" << std::endl;
	std::cerr << "  Reference                   " << std::endl;
	std::cerr << "  Naive                       " << std::endl;
	std::cerr << "  Proportional (default)      " << std::endl;
	std::cerr << "  Lookahead                   " << std::endl;
	std::cerr << "Supported file formats:" << std::endl;
	std::cerr << "  .real                       " << std::endl;
	std::cerr << "  .qasm                       " << std::endl;
}

int main(int argc, char** argv){
	if (argc < 3 || argc > 5) {
		show_usage(argv[0]);
		return 1;
	}

	// get filenames
	std::string file1 = argv[1];
	std::string file2 = argv[2];

	// get file format
	qc::Format format1;
	size_t dot = file1.find_last_of('.');
	std::string extension = file1.substr(dot+1);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
	if (extension == "real") {
		format1 = qc::Real;
	} else if (extension == "qasm") {
		format1 = qc::OpenQASM;
	} else {
		show_usage(argv[0]);
		return 1;
	}

	qc::Format format2;
	dot = file2.find_last_of('.');
	extension = file2.substr(dot+1);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
	if (extension == "real") {
		format2 = qc::Real;
	} else if (extension == "qasm") {
		format2 = qc::OpenQASM;
	} else {
		show_usage(argv[0]);
		return 1;
	}

	// get method
	ec::Method method = ec::Proportional;
	if (argc >= 4){
		std::string target_method = argv[3];
		std::transform(target_method.begin(), target_method.end(), target_method.begin(), [](unsigned char c) { return std::tolower(c); });

		if (target_method == "reference") {
			method = ec::Reference;
		} else if (target_method == "naive") {
			method = ec::Naive;
		} else if (target_method == "proportional") {
			method = ec::Proportional;
		} else if (target_method == "lookahead") {
			method = ec::Lookahead;
		} else {
			show_usage(argv[0]);
			return 1;
		}
	}

	if (argc >= 5) {
		std::string cmd = argv[4];
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return std::tolower(c); });

		if (cmd != "--print_csv") {
			show_usage(argv[0]);
			return 1;
		}
	}

	// read circuits
	qc::QuantumComputation qc1;
	qc1.import(file1, format1);

	qc::QuantumComputation qc2;
	qc2.import(file2, format2);

	// perform equivalence check
	ec::ImprovedDDEquivalenceChecker ec(qc1, qc2, method);
	ec.expectNothing();
	ec.check();

	if (argc < 5) {
		ec.printResult(std::cout);
	} else {
		ec.printCSVEntry(std::cout);
	}

	return 0;
}
