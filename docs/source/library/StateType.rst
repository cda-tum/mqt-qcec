State Types for Simulation
==========================

The type of states that is used in the :ref:`simulation checker <EquivalenceChecking:Simulation Equivalence Checker (using Decision Diagrams)>` allows to trade off efficiency versus performance.

* Classical stimuli (i.e., random *computational basis states*) already offer extremely high error detection rates in general and are comparatively fast to simulate, which makes them the default.

* Local quantum stimuli (i.e., random *single-qubit basis states*) are a little bit more computationally intensive, but provide even better error detection rates.

* Global quantum stimuli (i.e., random  *stabilizer states*) offer the highest available error detection rate, while at the same time incurring the highest computational effort.

For details, see :cite:p:`burgholzer2021randomStimuliGenerationQuantum`.

    .. autoclass:: mqt.qcec.types.StateTypeName

    .. autoclass:: mqt.qcec.StateType
        :undoc-members:
        :members:
