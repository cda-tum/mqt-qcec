Verify Compilation
==================

QCEC provides a dedicated :func:`.verify_compilation` method for verifying that a quantum circuit has been compiled correctly based on the methods proposed in :cite:p:`burgholzer2020verifyingResultsIBM`.

    .. currentmodule:: mqt.qcec
    .. autofunction:: verify_compilation

.. note::

    It is essential to include measurements at the end of the circuit, since the equivalence checker uses the measurements to determine the final location of the logical qubits in the compiled circuit.
    Failing to do so may result in incorrect results because the checker will then simply assume that the logical qubits are mapped to the physical qubits in the same order as they appear in the circuit.
    Make sure to insert measurements *before* the circuit is compiled to the target architecture.

    .. autoclass:: AncillaMode
        :undoc-members:
        :members:

Compilation Flow Profile Generation
###################################

QCEC provides dedicated compilation flow profiles for IBM Qiskit which can be used to efficiently verify the results of compilation flow results :cite:p:`burgholzer2020verifyingResultsIBM`.
These profiles are generated from IBM Qiskit using the :func:`.generate_profile` method.

    .. autofunction:: generate_profile
