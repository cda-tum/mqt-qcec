Results
=======

.. currentmodule:: jkq.qcec

This class captures the main results and statistics from :func:`~jkq.qcec.EquivalenceCheckingManager.run`.

    .. autoclass:: jkq.qcec.EquivalenceCheckingManager.Results

Most importantly, it provides the preprocessing as well as the equivalence checking time and the obtained results.

    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.preprocessing_time
    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.check_time
    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.equivalence

Furthermore, there is some information on the conducted simulations.

    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.started_simulations
    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.performed_simulations

If configured, it also includes state vector representations of the state used as :attr:`input <Configuration.Simulation.store_cex_input>` and the two :attr:`resulting states <Configuration.Simulation.store_cex_output>` in case a counterexample is obtained by any simulation.

    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.cex_input
    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.cex_output1
    .. autoattribute:: jkq.qcec.EquivalenceCheckingManager.Results.cex_output2

Last but not least, the class provides some convenience functions to check and print the result.

    .. automethod:: jkq.qcec.EquivalenceCheckingManager.Results.considered_equivalent
    .. automethod:: jkq.qcec.EquivalenceCheckingManager.Results.json
    .. automethod:: jkq.qcec.EquivalenceCheckingManager.Results.__repr__
