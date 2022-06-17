State Types for Simulation
==========================

The type of states that is used in the :attr:`simulation checker <mqt.qcec.Configuration.Execution.run_simulation_checker>` allows to trade off efficiency versus performance.

* Random *computational basis states* (referred to as *classical* stimuli in :cite:p:`burgholzer2021randomStimuliGenerationQuantum`) already offer extremely high error detection rates in general and are comparatively fast to simulate, which makes them the default.

* Random *single-qubit basis states* (referred to as *local quantum stimuli* in :cite:p:`burgholzer2021randomStimuliGenerationQuantum`) are a little bit more computationally intensive, but provide even better error detection rates.

* Random  *stabilizer states* (referred to as *global quantum stimuli* in :cite:p:`burgholzer2021randomStimuliGenerationQuantum`) offer the highest available error detection rate, while at the same time incurring the highest computational effort.

For details, see :cite:p:`burgholzer2021randomStimuliGenerationQuantum`.

    .. autoclass:: mqt.qcec.StateType
        :undoc-members:
        :members:
