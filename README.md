[![Build Status](https://travis-ci.com/burgholzer/QCEC.svg?token=md6M7jxUi7wqb4j9saQb&branch=master)](https://travis-ci.com/burgholzer/QCEC)
[![codecov](https://codecov.io/gh/burgholzer/QCEC/branch/master/graph/badge.svg?token=Ws9Rob367B)](https://codecov.io/gh/burgholzer/QCEC)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# QCEC - A tool for **Q**uantum **C**ircuit **E**quivalence **C**hecking

A tool for quantum circuit equivalence checking by the [Institute for Integrated Circuits](http://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at).

Developers: Lukas Burgholzer and Robert Wille.

The tool builds upon [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git) as well as [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git).

For more information, please visit [iic.jku.at/eda/research/quantum](https://iic.jku.at/eda/research/quantum_verification).

If you have any questions, feel free to contact us via [iic_quantum@jku.at](mailto:iic_quantum@jku.at) or by creating an issue on GitHub.

## Usage

This tool can be used for checking the equivalence of two quantum circuits provided in any of the following formats:
* `Real` (e.g. from [RevLib](http://revlib.org)),
* `OpenQASM` (e.g. used by IBM's [Qiskit](https://github.com/Qiskit/qiskit)),

with the following available methods:
   - Reference,
   - Naive,
   - Proportional,
   - Lookahead.

For details on the available methods we refer to [iic.jku.at/eda/research/quantum_verification](https://iic.jku.at/eda/research/quantum_verification).

It can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.
- The standalone executable is launched in the following way:
    ```commandline
    QCEC_app <PATH_TO_FILE_1> <PATH_TO_FILE_2> (<method>)
    ```
  where *\<method\>* is one of
   - reference
   - naive
   - proportional (**default**)
   - lookahead 
   
   The executable performs the equivalence check and prints its result to the standard output.
   
- The library can be used by including the ```ImprovedDDEquivalenceChecker.hpp``` header file and
    ```c++
    std::string file1 = "PATH_TO_FILE_1";
    qc::Format format1 = qc::{ Real | OpenQASM };
    qc::QuantumComputation qc1;
    qc1.import(file1, format1);
    
    std::string file2 = "PATH_TO_FILE_2";
    qc::Format format2 = qc::{ Real | OpenQASM };
    qc::QuantumComputation qc2;
    qc2.import(file2, format2);
    
    ec::Method method = ec::{ Naive | Proportional | Lookahead };
    auto eq = ec::ImprovedDDEquivalenceChecker(qc1, qc2, method);
    eq.check();
    eq.printResult(std::cout);
    ```
    Results may also be accessed through the ```eq.results``` member.

### System requirements

Building (and running) is continuously tested under Linux (Ubuntu 18.04) using gcc-7.4, gcc-9 and clang-9 and MacOS (Mojave 10.14) using AppleClang and gcc-9. 
However, the implementation should be compatible with any current C++ compiler supporting C++14 and a minimum CMake version of 3.10.

### Build and Run
For building the library alone the CMake target `QCEC` is available, i.e.,
```commandline
mkdir build && cd build
cmake ..
cmake --build . --target QCEC
```

To build the library and run a small demo, showcasing the tool's features, build the `QCEC_example` CMake target and run the resulting executable:

```commandline
cmake --build . --target QCEC_example
./QCEC_example
```

The standalone executable is built via:

```commandline 
cmake --build . --target QCEC_app
```

The repository also includes some (rudimentary) unit tests (using GoogleTest), which aim to ensure the correct behaviour of the tool. They can be built and executed in the following way:
```commandline
cmake --build . --target QCEC_test
./QCEC_test
```

## Reference

If you use our tool for your research, we will be thankful if you refer to it by citing the following publication:

```bibtex
@article{
    title={Improved DD-based Equivalence Checking of Quantum Circuits},
    author={Burgholzer, Lukas and Wille, Robert},
    journal={Asia and South Pacific Design Automation Conference (ASP-DAC)},
    year={2020}
}
```
