[![Build Status](https://travis-ci.com/iic-jku/qcec.svg?branch=master)](https://travis-ci.com/iic-jku/qcec)
[![codecov](https://codecov.io/gh/iic-jku/qcec/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/qcec)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![arXiv](https://img.shields.io/static/v1?label=arXiv&message=2004.08420&color=inactive)](https://arxiv.org/abs/2004.08420) 

# QCEC - A tool for **Q**uantum **C**ircuit **E**quivalence **C**hecking

A tool for quantum circuit equivalence checking by the [Institute for Integrated Circuits](http://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at) based on methods proposed in [[1]](https://arxiv.org/abs/2004.08420), [[2]](https://arxiv.org/abs/2009.02376). 

[[1]](https://arxiv.org/abs/2004.08420) L. Burgholzer and R. Wille. **"Advanced Equivalence Checking for Quantum Circuits"**. arXiv:2004.08420, 2020

[[2]](https://arxiv.org/abs/2009.02376) L. Burgholzer, R. Raymond, and R. Wille. **"Verifying Results of the IBM Qiskit Quantum Circuit Compilation Flow"**. In International Conference on Quantum Computing and Engineering (QCE), 2020

This tool can be used for checking the equivalence of two quantum circuits provided in any of the following formats:
 * `Real` (e.g. from [RevLib](http://revlib.org)),
 * `OpenQASM` (e.g. used by IBM's [Qiskit](https://github.com/Qiskit/qiskit)),
 * `TFC` (e.g. from [Reversible Logic Synthesis Benchmarks Page](http://webhome.cs.uvic.ca/~dmaslov/mach-read.html))
 
 with the following available methods:
- **Reference** - Construct and compare the DD for both circuits [[1, Section III.B]](https://arxiv.org/pdf/2004.08420.pdf#page=5),
- ![G \rightarrow \mathbb{I} \leftarrow G'](https://render.githubusercontent.com/render/math?math=G%20%5Crightarrow%20%5Cmathbb%7BI%7D%20%5Cleftarrow%20G') - Starting from the identity *I*, either apply gates from *G* or (inverted) gates from *G'* according to one of the following strategies [[1, Section IV.A]](https://arxiv.org/pdf/2004.08420.pdf#page=6)
    - **Naive** - Alternate between applications of *G* and *G'* [[1, Section V.A]](https://arxiv.org/pdf/2004.08420.pdf#page=8),
    - **Proportional** - Proportionally apply gates according to the gate count ratio of *G* and *G'* [[1, Section V.B]](https://arxiv.org/pdf/2004.08420.pdf#page=8),
    - **Lookahead** - Always apply the gate yielding the smaller DD [[1, Section V.C]](https://arxiv.org/pdf/2004.08420.pdf#page=8),
- **Simulation** - Conduct simulation runs to prove non-equivalence or give a strong indication of equivalence [[1, Section IV.B]](https://arxiv.org/pdf/2004.08420.pdf#page=7),
- **Verification of compilation results** - A dedicated scheme for verifying results of the IBM Qiskit Compilation Flow explicitly exploiting certain knowledge about the compilation process. [[2]](https://arxiv.org/abs/2009.02376)

The tool builds upon [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git) as well as [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git). For more information, please visit [iic.jku.at/eda/research/quantum_verification](http://iic.jku.at/eda/research/quantum_verification). 

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an issue on GitHub.

## Usage

This tool can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.
- The standalone executable is launched in the following way:
    ```commandline
    qcec_app <PATH_TO_FILE_1> <PATH_TO_FILE_2> (<method>)
    ```
  where *\<method\>* is one of
   - reference
   - naive
   - proportional (**default**)
   - lookahead 
   - simulation
   - compilationflow
   
  The ```simulation``` method has two optional parameters ```--nsims r``` and ```--fid F```, controlling the maximum number of simulations *r* (default: *16*) and the considered fidelity limit *F* (default *0.999*), respectively.
      
   The executable performs the equivalence check and prints its result to the standard output. If the `--csv` option is present, a CSV entry according to the following header is printed
    ```csv
   filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive;nsims
   ```
   
- Internally the library works in the following way
    - Import both input files into a `qc::QuantumComputation` object
        ```c++
        std::string file1 = "PATH_TO_FILE_1{.real | .qasm | .tfc}";
        qc::QuantumComputation qc1(file1);
        
        std::string file2 = "PATH_TO_FILE_2{.real | .qasm | .tfc}";
        qc::QuantumComputation qc2(file2);
        ```
    - Instantiate an `ec::EquivalenceChecker` object with both circuits
        ```c++
        ec::Method method = ec::{ Reference | Naive | Proportional | Lookahead };
        auto eq = ec::ImprovedDDEquivalenceChecker(qc1, qc2, method);
        ```
        or 
        ```c++ 
        auto eq = ec::PowerOfSimulationEquivalenceChecker(qc1, qc2);
        ```
        or 
        ```c++ 
        auto eq = ec::CompilationFlowEquivalenceChecker(qc1, qc2);
        ```
    - Perform the actual equivalence check
        ```c++
        eq.check();
        ```
    - Print the results
        ```c++
        eq.printResult();
        ```
        or access them through the ```eq.results``` member.
  
### System requirements

Building (and running) is continuously tested under Linux (Ubuntu 18.04) using gcc-7.4, gcc-10 and clang-9, MacOS (Mojave 10.15) using AppleClang and gcc-10, and Windows using MSVC 15.9. 
However, the implementation should be compatible with any current C++ compiler supporting C++14 and a minimum CMake version of 3.10.

### Configure, Build, and Install

In order to build the library execute the following in the project's main directory
1) Configure CMake
    ```commandline
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    ```
   Windows users using Visual Studio and the MSVC compiler may try
   ```commandline
   cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release
   ```
   Older CMake versions not supporting the above syntax (< 3.13) may be used with
   ```commandline
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```
2) Build the respective target. 
    ```commandline
   cmake --build ./build --config Release --target <target>
   ```
    The following CMake targets are available
    - `qcec_app`: The commandline executable
    - `qcec`: The standalone library
    - `qcec_example`: A small commandline demo example
    - `qcec_test`: Unit tests using GoogleTest

3) Optional: The QCEC library and tool may be installed on the system by executing
   
    ```commandline
    cmake --build ./build --config Release --target install
    ```

    It can then also be included in other projects using the following CMake snippet
    
    ```cmake
    find_package(qcec)
    target_link_libraries(${TARGET_NAME} PRIVATE JKQ::qcec)
    ```

## Reference

If you use our tool for your research, we will be thankful if you refer to it by citing the appropriate publication (provided in BibTeX or biblatex style):

<details open>
<summary>[1] L. Burgholzer and R. Wille. "Advanced Equivalence Checking for Quantum Circuits". arXiv:2004.08420, 2020</summary>

<details>
<summary>bibtex</summary>

```bibtex
@misc{burgholzer2020advanced,
    author = {Burgholzer, Lukas and Wille, Robert},
    title = {Advanced Equivalence Checking of Quantum Circuits},
    archivePrefix = "arXiv", 
    note = {arXiv:2004.08420},
    year = 2020
}
```

</details>

<details>
<summary>biblatex</summary>

```bibtex 
@online{burgholzer2020advanced,
    author       = {Burgholzer, Lukas and Wille, Robert},
    title        = {Advanced Equivalence Checking of Quantum Circuits},
    date         = {2020-04-21},
    eprinttype   = {arxiv},
    eprint       = {2004.08420},
    eprintclass  = {quant-ph},
}
```

</details>

</details>

<details open>
<summary>[2] L. Burgholzer, R. Raymond, and R. Wille. "Verifying Results of the IBM Qiskit Quantum Circuit Compilation Flow". In International Conference on Quantum Computing and Engineering (QCE), 2020</summary>

<details>
<summary>bibtex</summary>

```bibtex
@inproceedings{burgholzer2020verifyingResultsIBM,
  title = {Verifying results of the {{IBM Qiskit}} quantum circuit compilation flow},
  booktitle = {International Conference on Quantum Computing and Engineering},
  author = {Burgholzer, Lukas and Raymond, Rudy and Wille, Robert},
  year = {2020}
}
```

</details>

<details>
<summary>biblatex</summary>

```bibtex 
@inproceedings{burgholzer2020verifyingResultsIBM,
  title = {Verifying results of the {{IBM Qiskit}} quantum circuit compilation flow},
  author = {Burgholzer, Lukas and Raymond, Rudy and Wille, Robert},
  date = {2020},
  eventtitle = {International Conference on Quantum Computing and Engineering}
}
```

</details>

</details>
