[![Build Status](https://travis-ci.com/iic-jku/qcec.svg?branch=master)](https://travis-ci.com/iic-jku/qcec)
[![codecov](https://codecov.io/gh/iic-jku/qcec/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/qcec)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# QCEC - A tool for **Q**uantum **C**ircuit **E**quivalence **C**hecking

A tool for quantum circuit equivalence checking by the [Institute for Integrated Circuits](http://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at).

Developers: Lukas Burgholzer and Robert Wille.

The tool builds upon [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git) as well as [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git).

For more information, please visit [iic.jku.at/eda/research/quantum_verification](http://iic.jku.at/eda/research/quantum_verification).

If you have any questions, feel free to contact us via [iic_quantum@jku.at](mailto:iic_quantum@jku.at) or by creating an issue on GitHub.

## Usage

This tool can be used for checking the equivalence of two quantum circuits provided in any of the following formats:
* `Real` (e.g. from [RevLib](http://revlib.org)),
* `OpenQASM` (e.g. used by IBM's [Qiskit](https://github.com/Qiskit/qiskit)),

with the following available methods:
   - Reference,
   - Naive,
   - Proportional,
   - Lookahead,
   - CompilationFlow (as of version 1.1),
   - PowerOfSimulation (as of version 1.2).

For details on the available methods we refer to [iic.jku.at/eda/research/quantum_verification](http://iic.jku.at/eda/research/quantum_verification).

It can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.
- The standalone executable is launched in the following way:
    ```commandline
    QCEC_app <PATH_TO_FILE_1> <PATH_TO_FILE_2> (<method>) (--print_csv)
    ```
  where *\<method\>* is one of
   - reference
   - naive
   - proportional 
   - lookahead 
   - compilationflow (**default**)
   - powerofsimulation
   
   The executable performs the equivalence check and prints its result to the standard output. If the `--print_csv` option is present, a CSV entry according to the following header is printed
   
    ```csv
    filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive;nsims
    ```
   
- The library can be used by including, e.g., the appropriate header file and
    ```c++
    std::string file1 = "PATH_TO_FILE_1{.real | .qasm}";
    qc::QuantumComputation qc1(file1);
    
    std::string file2 = "PATH_TO_FILE_2{.real | .qasm}";
    qc::QuantumComputation qc2(file2);
    
    ec::Method method = ec::{ Naive | Proportional | Lookahead };
    auto eq = ec::ImprovedDDEquivalenceChecker(qc1, qc2, method);
    ```
  ```c++ 
  auto eq = ec::CompilationFlowEquivalenceChecker(qc1, qc2);
  ```
  or 
  ```c++ 
  auto eq = ec::PowerOfSimulationEquivalenceChecker(qc1, qc2);
  ```
  and then calling
  ```c++
  eq.check(ec::Configuration{});
  eq.printResult(std::cout);
  ```
    Results may also be accessed through the ```eq.results``` member.

### System requirements

Building (and running) is continuously tested under Linux (Ubuntu 18.04) using gcc-7.4, gcc-9 and clang-9, MacOS (Mojave 10.14) using AppleClang and gcc-9, and Windows using MSVC 15.9. 
However, the implementation should be compatible with any current C++ compiler supporting C++14 and a minimum CMake version of 3.10.

### Build and Run
For building the library alone the CMake target `QCEC` is available, i.e.,
```commandline
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release  --target QCEC
```

Windows users using Visual Studio and the MSVC compiler need to build the project with
```commandline
mkdir build && cd build
cmake .. -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target QCEC
```

To build the library and run a small demo, showcasing the tool's features, build the `QCEC_example` CMake target and run the resulting executable:

```commandline
cmake --build . --config Release --target QCEC_example
./QCEC_example
```

The standalone executable is built via:

```commandline 
cmake --build . --target QCEC_app --config Release
```

The repository also includes some unit tests (using GoogleTest), which aim to ensure the correct behaviour of the tool. They can be built and executed in the following way:
```commandline
cmake --build . --config Release --target QCEC_test
./QCEC_test
```

The QCEC library and tool may be installed on the system by executing

```commandline
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target install
```

It can then also be included in other projects using the following CMake snippet

```cmake
find_package(QCEC)
target_link_libraries(${TARGET_NAME} PRIVATE EC::QCEC)
```

## Reference

If you use our tool for your research, we will be thankful if you refer to it by citing the following publication:

```bibtex
@article{burgholzer2020improved,
    title={Improved DD-based Equivalence Checking of Quantum Circuits},
    author={Burgholzer, Lukas and Wille, Robert},
    journal={Asia and South Pacific Design Automation Conference (ASP-DAC)},
    year={2020}
}
```
