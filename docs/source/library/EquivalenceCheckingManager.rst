Equivalence Checking Manager
============================
This is the main class of QCEC that allows to check the equivalence of quantum circuits based on the methods proposed in :cite:p:`burgholzer2021advanced`. It features many configuration options that orchestrate the whole procedure. This page describes all the relevant methods of the Equivalence Checking Manager.

    .. currentmodule:: mqt.qcec
    .. autoclass:: EquivalenceCheckingManager

Constructing an instance
########################
The simplest way of constructing an :class:`EquivalenceCheckingManager` is to just pass it the two circuits whose equivalence shall be checked.

    .. code-block:: python

       ecm = EquivalenceCheckingManager(circ1=qc1, circ2=qc2)

This constructs the manager using all the default options. The circuits to be verified can be provided in various ways:

* Python objects:

    * :class:`qiskit.circuit.QuantumCircuit` from IBM's `Qiskit <https://github.com/Qiskit/qiskit>`_ **(preferred)**
    * `QasmQobjExperiment` from IBM's `Qiskit <https://github.com/Qiskit/qiskit>`_

* Files

    * `OpenQASM 2.0` and (a subset of) `OpenQASM 3.0` (e.g. used by IBM's `Qiskit <https://github.com/Qiskit/qiskit>`_),
    * `Real` (e.g. from `RevLib <http://revlib.org>`_),
    * `TFC` (e.g. from `Reversible Logic Synthesis Benchmarks Page <http://webhome.cs.uvic.ca/~dmaslov/mach-read.html>`_)
    * `QC` (e.g. from `Feynman <https://github.com/meamy/feynman>`_)

It can be further configured by passing a :class:`~.Configuration` object to the constructor.

    .. automethod:: EquivalenceCheckingManager.__init__

Configuration after instantiation
#################################
In addition, the :class:`~.Configuration` of the manager can be altered after its construction. To this end, several convenience functions are provided which allow to modify the individual options:

* :class:`Execution Options <Configuration.Execution>`:
    These options orchestrate the :func:`~EquivalenceCheckingManager.run` method.

        .. automethod:: EquivalenceCheckingManager.set_parallel
        .. automethod:: EquivalenceCheckingManager.set_nthreads
        .. automethod:: EquivalenceCheckingManager.set_timeout
        .. automethod:: EquivalenceCheckingManager.set_construction_checker
        .. automethod:: EquivalenceCheckingManager.set_simulation_checker
        .. automethod:: EquivalenceCheckingManager.set_alternating_checker
        .. automethod:: EquivalenceCheckingManager.set_zx_checker
        .. automethod:: EquivalenceCheckingManager.set_tolerance

* :class:`Optimizations <Configuration.Optimizations>`
    These functions allow to apply specific circuit optimizations that might not have been performed during initialization. Note that already performed optimizations cannot be reverted since they are applied at construction time.

        .. automethod:: EquivalenceCheckingManager.fuse_single_qubit_gates
        .. automethod:: EquivalenceCheckingManager.reconstruct_swaps
        .. automethod:: EquivalenceCheckingManager.reorder_operations

* :class:`Application Options <Configuration.Application>`
    These options describe the :class:`Application Scheme <ApplicationScheme>` that is used for the individual equivalence checkers (based on decision diagrams). The scheme can either be set collectively for all checkers at once or individually.

        .. automethod:: EquivalenceCheckingManager.set_application_scheme
        .. automethod:: EquivalenceCheckingManager.set_construction_application_scheme
        .. automethod:: EquivalenceCheckingManager.set_simulation_application_scheme
        .. automethod:: EquivalenceCheckingManager.set_alternating_application_scheme

    The :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application scheme can be configured with a profile that specifies the cost of gates. Again, this can be set collectively for all checkers or individually.

        .. automethod:: EquivalenceCheckingManager.set_gate_cost_profile
        .. automethod:: EquivalenceCheckingManager.set_construction_gate_cost_profile
        .. automethod:: EquivalenceCheckingManager.set_simulation_gate_cost_profile
        .. automethod:: EquivalenceCheckingManager.set_alternating_gate_cost_profile

* :class:`Functionality Options <Configuration.Functionality>`
    These options influence all checkers that consider the whole functionality of a circuit.

        .. automethod:: EquivalenceCheckingManager.set_trace_threshold

* :class:`Simulation Options <Configuration.Simulation>`
    These options influence the :ref:`simulation checker <EquivalenceChecking:Simulation Equivalence Checker (using Decision Diagrams)>`.

        .. automethod:: EquivalenceCheckingManager.set_fidelity_threshold
        .. automethod:: EquivalenceCheckingManager.set_max_sims
        .. automethod:: EquivalenceCheckingManager.set_state_type
        .. automethod:: EquivalenceCheckingManager.set_seed
        .. automethod:: EquivalenceCheckingManager.store_cex_input
        .. automethod:: EquivalenceCheckingManager.store_cex_output

Running the equivalence check
##############################
Once the manager has been constructed and (optionally) configured, the equivalence check can be started by calling :func:`~EquivalenceCheckingManager.run`.

    .. automethod:: EquivalenceCheckingManager.run

Obtaining the results
#####################
After the run has completed, several results can be obtained:

* The final result of the equivalence check.

    .. automethod:: EquivalenceCheckingManager.equivalence

* The :class:`EquivalenceCheckingManager.Results` object that also contains statistics such as runtime and performed simulations.

    .. automethod:: EquivalenceCheckingManager.get_results

* A JSON-style dictionary containing all available information.

    .. automethod:: EquivalenceCheckingManager.Results.json
