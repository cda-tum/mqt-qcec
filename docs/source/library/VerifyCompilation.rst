Verify Compilation
==================

QCEC provides a dedicated :code:`verify_compilation` method for verifying that a quantum circuit has been compiled correctly based on the methods proposed in :cite:p:`burgholzer2020verifyingResultsIBM`.

    .. currentmodule:: mqt.qcec
    .. automethod:: mqt.qcec::verify_compilation

    .. autoclass:: mqt.qcec.AncillaMode
        :undoc-members:
        :members:

Compilation Flow Profile Generation
###################################

QCEC provides dedicated compilation flow profiles for IBM Qiskit which can be used to efficiently verify the results of compilation flow results :cite:p:`burgholzer2020verifyingResultsIBM`.
These profiles are generated from IBM Qiskit using the :code:`generate_profile` method.

    .. automethod:: mqt.qcec::generate_profile
