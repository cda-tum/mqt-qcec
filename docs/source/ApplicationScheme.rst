Application Scheme
==================

An *application scheme* describes the order in which the individual operations of both circuits are applied during the equivalence check.

In case of the alternating equivalence checker, this is the key component to allow the intermediate decision diagrams to remain close to the identity. In order to efficiently verify the results of compilation flows (see :cite:p:`burgholzer2020verifyingResultsIBM`), choose the :attr:`~.ApplicationScheme.gate_cost` scheme and optionally provide it with a dedicated :attr:`profile <qcec.Configuration.Application.profile>`.

In case of the other checkers, which consider both circuits individually, using a non-sequential application scheme can significantly boost the operation caching performance in the underlying decision diagram package.

    .. autoclass:: qcec.ApplicationScheme
        :undoc-members:
        :members:
