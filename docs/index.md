# MQT QCEC - A tool for Quantum Circuit Equivalence Checking

```{raw} latex
\begin{abstract}
```

MQT QCEC is an open-source C++17 and Python library for {doc}`quantum circuit equivalence checking <equivalence_checking>` developed as part of the _{doc}`Munich Quantum Toolkit (MQT) <mqt:index>`_ {cite:p}`mqt` by the [Chair for Design Automation](https://www.cda.cit.tum.de/) at the [Technical University of Munich](https://www.tum.de/).

This documentation provides a comprehensive guide to the MQT QCEC library, including {doc}`installation instructions <installation>`, a {doc}`quickstart guide <quickstart>`, and detailed {doc}`API documentation <api/mqt/qcec/index>`.
The source code of MQT QCEC is publicly available on GitHub at [cda-tum/mqt-qcec](https://github.com/cda-tum/mqt-qcec), while pre-built binaries are available via [PyPI](https://pypi.org/project/mqt.qcec/) for all major operating systems and all modern Python versions.
MQT QCEC is fully compatible with Qiskit 1.0 and above.

````{only} latex
```{note}
A live version of this document is available at [mqt.readthedocs.io/projects/qcec](https://mqt.readthedocs.io/projects/qcec).
```
````

```{raw} latex
\end{abstract}

\sphinxtableofcontents
```

```{toctree}
:hidden:

self
```

```{toctree}
:maxdepth: 2
:caption: User Guide

installation
quickstart
equivalence_checking
compilation_flow_verification
parametrized_circuits
partial_equivalence
references
```

````{only} not latex
```{toctree}
:maxdepth: 2
:titlesonly:
:caption: Developers
:glob:

contributing
support
development_guide
```
````

```{toctree}
:hidden:
:maxdepth: 6
:caption: API Reference

api/mqt/qcec/index
```
