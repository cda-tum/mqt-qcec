Results
=======

This class captures the main results and statistics from :func:`~qcec.EquivalenceCheckingManager.run`.

    .. autoclass:: qcec.EquivalenceCheckingManager.Results

Most importantly, it provides the preprocessing as well as the equivalence checking time and the obtained results.

    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.preprocessing_time
    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.check_time
    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.equivalence

Furthermore, there is some information on the conducted simulations.

    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.started_simulations
    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.performed_simulations

If configured, it also includes state vector representations of the state used as :attr:`input <qcec.Configuration.Simulation.store_cex_input>` and the two :attr:`resulting states <qcec.Configuration.Simulation.store_cex_output>` in case a counterexample is obtained by any simulation.

    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.cex_input
    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.cex_output1
    .. autoattribute:: qcec.EquivalenceCheckingManager.Results.cex_output2

Last but not least, the class provides some convenience functions to check and print the result.

    .. automethod:: qcec.EquivalenceCheckingManager.Results.considered_equivalent
    .. automethod:: qcec.EquivalenceCheckingManager.Results.json
    .. automethod:: qcec.EquivalenceCheckingManager.Results.__repr__
