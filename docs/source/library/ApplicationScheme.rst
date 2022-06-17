Application Scheme
==================

An *application scheme* describes the order in which the individual operations of both circuits are applied during the equivalence check.

In case of the alternating equivalence checker, this is the key component to allow the intermediate decision diagrams to remain close to the identity (as proposed in :cite:p:`burgholzer2021advanced`).
See :doc:`/CompilationFlowVerification` for more information on the dedicated application scheme for verifying compilation flow results (as proposed in :cite:p:`burgholzer2020verifyingResultsIBM`).

In case of the other checkers, which consider both circuits individually, using a non-sequential application scheme can significantly boost the operation caching performance in the underlying decision diagram package.

    .. autoclass:: mqt.qcec.ApplicationScheme
        :undoc-members:
        :members:
