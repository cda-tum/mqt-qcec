[![PyPI](https://img.shields.io/pypi/v/jkq.qcec?logo=pypi&style=plastic)](https://pypi.org/project/jkq.qcec/)
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/iic-jku/qcec/CI?logo=github&style=plastic)](https://github.com/iic-jku/qcec/actions?query=workflow%3A%22CI%22)
[![Codecov branch](https://img.shields.io/codecov/c/github/iic-jku/qcec/master?label=codecov&logo=codecov&style=plastic)](https://codecov.io/gh/iic-jku/qcec)
![GitHub](https://img.shields.io/github/license/iic-jku/qcec?style=plastic)
[![toolset: JKQ](https://img.shields.io/badge/toolset-JKQ-blue?style=plastic)](https://github.com/iic-jku/jkq)
[![arXiv](https://img.shields.io/static/v1?label=arXiv&message=2004.08420&color=inactive&style=plastic)](https://arxiv.org/abs/2004.08420)
[![arXiv](https://img.shields.io/static/v1?label=arXiv&message=2009.02376&color=inactive&style=plastic)](https://arxiv.org/abs/2009.02376)
[![arXiv](https://img.shields.io/static/v1?label=arXiv&message=2011.07288&color=inactive&style=plastic)](https://arxiv.org/abs/2011.07288)

# QCEC - A JKQ tool for **Q**uantum **C**ircuit **E**quivalence **C**hecking

A JKQ tool for Quantum Circuit Equivalence Checking by the [Institute for Integrated Circuits](http://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at) based on methods proposed in [[1]](https://arxiv.org/abs/2004.08420), [[2]](https://arxiv.org/abs/2009.02376), [[3]](https://arxiv.org/abs/2011.07288). 

[[1]](https://arxiv.org/abs/2004.08420) L. Burgholzer and R. Wille. **"Advanced Equivalence Checking for Quantum Circuits"**. IEEE Transactions on Computer Aided Design of Integrated Circuits and Systems (TCAD), 2021 (pre-print [arXiv:2004.08420](https://arxiv.org/abs/2004.08420))

[[2]](https://arxiv.org/abs/2009.02376) L. Burgholzer, R. Raymond, and R. Wille. **"Verifying Results of the IBM Qiskit Quantum Circuit Compilation Flow"**. In International Conference on Quantum Computing and Engineering (QCE), 2020 (pre-print [arXiv:2009.02376](https://arxiv.org/abs/2009.02376))

[[3]](https://arxiv.org/abs/2011.07288) L. Burgholzer, R. Kueng, and R. Wille. **"Random Stimuli Generation for the Verification of Quantum Circuits"**. In Asia and South Pacific Design Automation Conference (ASP-DAC), 2021 (pre-print [arxiv:2011.07288](https://arxiv.org/abs/2011.07288)) 

This tool can be used for checking the equivalence of two quantum circuits provided in any of the following formats:
 * `QuantumCircuit` object from IBM's [Qiskit](https://github.com/Qiskit/qiskit) (only through the JKQ QCEC Python bindings)
 * `OpenQASM` (e.g. used by IBM's [Qiskit](https://github.com/Qiskit/qiskit)),
 * `Real` (e.g. from [RevLib](http://revlib.org)),
 * `TFC` (e.g. from [Reversible Logic Synthesis Benchmarks Page](http://webhome.cs.uvic.ca/~dmaslov/mach-read.html))
 * `QC` (e.g. from [Feynman](https://github.com/meamy/feynman))
 
 with the following available methods:
- **Reference** - Construct and compare the DD for both circuits [[1, Section III.B]](https://arxiv.org/pdf/2004.08420.pdf#page=5),
- ![G \rightarrow \mathbb{I} \leftarrow G'](https://render.githubusercontent.com/render/math?math=G%20%5Crightarrow%20%5Cmathbb%7BI%7D%20%5Cleftarrow%20G') - Starting from the identity *I*, either apply gates from *G* or (inverted) gates from *G'* according to one of the following strategies [[1, Section IV.A]](https://arxiv.org/pdf/2004.08420.pdf#page=6):
    - **Naive** - Alternate between applications of *G* and *G'* [[1, Section V.A]](https://arxiv.org/pdf/2004.08420.pdf#page=8),
    - **Proportional** - Proportionally apply gates according to the gate count ratio of *G* and *G'* [[1, Section V.B]](https://arxiv.org/pdf/2004.08420.pdf#page=8),
    - **Lookahead** - Always apply the gate yielding the smaller DD [[1, Section V.C]](https://arxiv.org/pdf/2004.08420.pdf#page=8),
- **Simulation** - Conduct simulation runs to prove non-equivalence or give a strong indication of equivalence [[1, Section IV.B]](https://arxiv.org/pdf/2004.08420.pdf#page=3) using: 
  - **Classical Stimuli** - computational basis states [[1, Section IV.B]](https://arxiv.org/pdf/2004.08420.pdf#page=7), [[3, Section 3.1]](https://arxiv.org/pdf/2011.07288.pdf#page=3)
  - **Local Quantum Stimuli** - each qubit value is independently chosen from any of the six basis states (|0>, |1>, |+>, |->, |L>, |R>) [[3, Section 3.2]](https://arxiv.org/pdf/2011.07288.pdf#page=4)
  - **Global Quantum Stimuli** - random stabilizer states [[3, Section 3.3]](https://arxiv.org/pdf/2011.07288.pdf#page=4)
- **Verification of compilation results** - A dedicated scheme for verifying results of the IBM Qiskit Compilation Flow explicitly exploiting certain knowledge about the compilation process. [[2]](https://arxiv.org/pdf/2009.02376.pdf)

The tool builds upon [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git) as well as [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git). For more information, please visit [iic.jku.at/eda/research/quantum_verification](http://iic.jku.at/eda/research/quantum_verification). If you want to visually explore decision diagrams for quantum computing, check out our installation-free web-tool [JKQ DDVis](https://iic.jku.at/eda/research/quantum_dd/tool/).

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an issue on [GitHub](https://guthub.com/iic-jku/qcec/issues).

## Usage

JKQ QCEC is mainly developed as a C++ library with an easy-to-use Python interface. 
- Get the Python package
    ```bash
    pip install jkq.qcec
    ```
  In order to make the library as easy to use as possible (without compilation), we provide wheels for most common platforms (64-bit Linux, MacOS, Windows). However, in order to get the best performance out of QCEC, it is recommended to
  build it locally from the source distribution via
    ```bash
    pip install --no-binary jkq.qcec
    ```
  This enables platform specific compiler optimizations that cannot be enabled on portable wheels.
- Start using it in Python:
    ```python
    from jkq.qcec import *
    
    config = Configuration()
    <...>  # set configuration options
    results = verify(circ1, circ2, config)
    ```
Both circuits can either be IBM Qiskit `QuantumCircuit` objects or paths to circuit files (in any of the formats listed above). 

The verification procedure can be configured with the following settings and options:
 - General settings:
    - `method`: Equivalence checking method to use
        - reference
        - ![G \rightarrow \mathbb{I} \leftarrow G'](https://render.githubusercontent.com/render/math?math=G%20%5Crightarrow%20%5Cmathbb%7BI%7D%20%5Cleftarrow%20G') (*default*)
        - simulation
    - `tolerance`: Numerical tolerance used during computation (`1e-13` per default)
- Settinggs for the ![G \rightarrow \mathbb{I} \leftarrow G'](https://render.githubusercontent.com/render/math?math=G%20%5Crightarrow%20%5Cmathbb%7BI%7D%20%5Cleftarrow%20G') method:
    - `strategy`: strategy to use for the scheme
        - naive
        - proportional (*default*)
        - lookahead
        - compilationflow
- Settings for the simulation-based method:
    - `fidelity`: Fidelity limit for comparison (`0.999` per default)
    - `max_sims`: Maximum number of simulations to conduct (`16` per default)
    - `stimuli_type`: Type of stimuli to use
        - classical (*default*)
        - localquantum
        - globalquantum
    - `store_cex_input`: Store counterexample input state vector (*off* by default)
    - `store_cex_output`: Store resulting counterexample state vectors (*off* by default)
- optimizations:
    - `reconstruct_swaps`: Reconstruct SWAP operations from consecutive CNOTs (*on* per default)
    - `fuse_single_qubit_gates`: Fuse consecutive single qubit gates (*on* per default)
    - `remove_diagonal_gates_before_measure`: Remove diagonal gates before measurements (*off* by default)
    
The `qcec.Results` class that is returned by the `verify` function provides `json()` and `csv()` methods to produce JSON or CSV formatted output.

### Integration of IBM Qiskit
The JKQ QCEC tool is designed to natively integrate with IBM Qiskit. In particular, using our tool to verify, e.g., the results of IBM Qiskit's quantum circuit compilation flow, is as easy as:
```python
from jkq.qcec import Configuration, Strategy, verify
from qiskit import QuantumCircuit, transpile

# create your quantum circuit
qc = <...> 

# append measurements to save output mapping of physical to logical (qu)bits
qc.measure_all() 

# compile circuit to appropriate backend using some optimization level
qc_comp = transpile(qc, backend=<...>, optimization_level=<0 | 1 | 2 | 3>) 

# set dedicated verification strategy
config = Configuration()
config.strategy = Strategy.compilationflow

# verify the compilation result
result = verify(qc, qc_comp, config)
```

### Command-line Executable
JKQ QCEC also provides a **standalone executable** with command-line interface called `qcec_app`.
It provides the same options as the Python module as flags (e.g., `--method <method>` for setting the method) and produces JSON formatted output.
For a full list of options, call `qcec_app --help`.

### System requirements

Building (and running) is continuously tested under Linux, MacOS, and Windows using the [latest available system versions for GitHub Actions](https://github.com/actions/virtual-environments).
However, the implementation should be compatible with any current C++ compiler supporting C++17 and a minimum CMake version of 3.14.

### Library Organisation
Internally the JKQ QCEC library works in the following way
- Import both input files into a `qc::QuantumComputation` object
    ```c++
    std::string file1 = "<PATH_TO_FILE_1>";
    qc::QuantumComputation qc1(file1);
    
    std::string file2 = "<PATH_TO_FILE_2>";
    qc::QuantumComputation qc2(file2);
    ```
- Instantiate an `ec::EquivalenceChecker` object with both circuits
    ```c++
    auto eq = ec::EquivalenceChecker(qc1, qc2);
    ```
    or
    ```c++
    auto eq = ec::ImprovedDDEquivalenceChecker(qc1, qc2);
    ```
    or
    ```c++ 
    auto eq = ec::SimulationBasedEquivalenceChecker(qc1, qc2);
    ```
    or 
    ```c++ 
    auto eq = ec::CompilationFlowEquivalenceChecker(qc1, qc2);
    ```
- Set configuration options, e.g.,
    ```c++
    ec::Configuration config{};
    config.tolerance = 1e-8;
    config.strategy = ec::Strategy::Lookahead;
    ```
- Perform the actual equivalence check
    ```c++
    auto results = eq.check(config);
    ```
- Print the results
    ```c++
    results.printJSON();
    ```

### Setup, Configure, and Build

To start off, clone this repository using
```shell
git clone --recurse-submodules -j8 https://github.com/iic-jku/qcec 
```
Note the `--recurse-submodules` flag. It is required to also clone all the required submodules. If you happen to forget passing the flag on your initial clone, you can initialize all the submodules by executing `git submodule update --init --recursive` in the main project directory.

Our projects use CMake as the main build configuration tool. Building a project using CMake is a two-stage process. First, CMake needs to be *configured* by calling
```shell 
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```
This tells CMake to search the current directory `.` (passed via `-S`) for a *CMakeLists.txt* file and process it into a directory `build` (passed via `-B`).
The flag `-DCMAKE_BUILD_TYPE=Release` tells CMake to configure a *Release* build (as opposed to, e.g., a *Debug* build).

After configuring with CMake, the project can be built by calling
```shell
 cmake --build build --config Release
```
This tries to build the project in the `build` directory (passed via `--build`).
Some operating systems and developer environments explicitly require a configuration to be set, which is why the `--config` flag is also passed to the build command. The flag `--parallel <NUMBER_OF_THREADS>` may be added to trigger a parallel build.

Building the project this way generates
- the main library `libqcec.a` (Unix) / `qcec.lib` (Windows) in the `build/src` directory
- the commandline executables `qcec_app` and `qcec_sim_app` (for simulation-based verification) in the `build/apps` directory
- a test executable `qcec_test` containing a small set of unit tests in the `build/test` directory (only if `-DBUILD_QCEC_TESTS=ON` is passed to CMake during configuration)

### Extending the Python Bindings

To extend the Python bindings you can locally install the package in edit mode, so that changes in the Python code are instantly available.
The following example assumes you have a [virtual environment](https://docs.python.org/3/library/venv.html) set up and activated.

```commandline
(venv) $ pip install cmake
(venv) $ pip install --editable .
```

If you change parts of the C++ code, you have to run the second line to make the changes visible in Python.

## Reference

If you use our tool for your research, we will be thankful if you refer to it by citing the appropriate publication:

<details open>
<summary>[1] L. Burgholzer and R. Wille. "Advanced Equivalence Checking for Quantum Circuits". IEEE Trans. on CAD of Integrated Circuits and Systems (TCAD), 2021</summary>

```bibtex
@article{burgholzer2021advanced,
    author = {Burgholzer, Lukas and Wille, Robert},
    title = {Advanced Equivalence Checking for Quantum Circuits},
    journaltitle = {{IEEE} Transactions on {CAD} of Integrated Circuits and Systems},
    year = {2021}
}
```

</details>

<details open>
<summary>[2] L. Burgholzer, R. Raymond, and R. Wille. "Verifying Results of the IBM Qiskit Quantum Circuit Compilation Flow". In International Conference on Quantum Computing and Engineering (QCE), 2020</summary>

```bibtex
@inproceedings{burgholzer2020verifyingResultsIBM,
  title = {Verifying results of the {{IBM Qiskit}} quantum circuit compilation flow},
  booktitle = {International Conference on Quantum Computing and Engineering},
  author = {Burgholzer, Lukas and Raymond, Rudy and Wille, Robert},
  year = {2020}
}
```

</details>

<details open>
<summary>[3] L. Burgholzer, R. Kueng, and R. Wille. "Random Stimuli Generation for the Verification of Quantum Circuits". Asia and South Pacific Design Automation Conference (ASP-DAC), 2021</summary>

```bibtex
@inproceedings{burgholzer2021randomStimuliGenerationQuantum,
  title = {Random stimuli generation for the verification of quantum circuits},
  booktitle = {Asia and South Pacific Design Automation Conference},
  author = {Burgholzer, Lukas and Richard, Kueng and Wille, Robert},
  year = {2021}
}
```

</details>
