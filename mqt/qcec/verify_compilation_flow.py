from __future__ import annotations

import sys
from typing import Any

if sys.version_info < (3, 10, 0):
    import importlib_resources as resources
else:
    from importlib import resources  # type: ignore[no-redef]

from mqt.qcec import ApplicationScheme, Configuration, EquivalenceCheckingManager
from mqt.qcec.compilation_flow_profiles import AncillaMode, generate_profile_name
from qiskit import QuantumCircuit

from .parameterized import __is_parameterized, check_parameterized


def verify_compilation(
    original_circuit: QuantumCircuit | str,
    compiled_circuit: QuantumCircuit | str,
    optimization_level: int = 1,
    ancilla_mode: AncillaMode = AncillaMode.NO_ANCILLA,
    configuration: Configuration | None = None,
    **kwargs: Any,
) -> EquivalenceCheckingManager.Results:
    """
    Similar to :func:`verify <.verify>`, but uses a dedicated compilation flow profile to guide the equivalence checking process.
    The compilation flow profile is determined by the ``optimization_level`` and ``ancilla_mode`` arguments.

    There are two ways of configuring the equivalence checking process:

    1. Pass a :class:`Configuration <.Configuration>` instance as the ``configuration`` argument.

    2. Pass keyword arguments to this function. These are directly passed to the :meth:`constructor <.EquivalenceCheckingManager.__init__>` of the :class:`EquivalenceCheckingManager`.

    :param original_circuit: The original circuit.
    :type original_circuit: QuantumCircuit | str
    :param compiled_circuit: The compiled circuit.
    :type compiled_circuit: QuantumCircuit | str
    :param optimization_level: The optimization level used for compiling the circuit (0, 1, 2, or 3).
    :type optimization_level: int
    :param ancilla_mode: The `ancilla mode <.AncillaMode>` used for realizing multi-controlled Toffoli gates, as available in Qiskit
    :type ancilla_mode: AncillaMode
    :param configuration: The configuration to use for the equivalence checking process.
    :type configuration: Configuration
    :param kwargs: Keyword arguments to pass to the :class:`EquivalenceCheckingManager <.EquivalenceCheckingManager>` constructor.
    :return: The results of the equivalence checking process.
    :rtype: EquivalenceCheckingManager.Results
    """

    # use the gate_cost scheme for the verification
    scheme = ApplicationScheme.gate_cost
    # get the pre-defined profile for the gate_cost scheme
    profile_name = generate_profile_name(optimization_level=optimization_level, mode=ancilla_mode)
    ref = resources.files("mqt.qcec") / "profiles" / profile_name

    if __is_parameterized(original_circuit) or __is_parameterized(compiled_circuit):
        if kwargs or configuration is None:
            with resources.as_file(ref) as path:
                return check_parameterized(
                    original_circuit,
                    compiled_circuit,
                    configuration,
                    construction_scheme="gate_cost",
                    profile=str(path),
                    **kwargs,
                )
        else:
            configuration.application.construction_scheme = scheme
            configuration.application.simulation_scheme = scheme
            configuration.application.alternating_scheme = scheme
            with resources.as_file(ref) as path:
                configuration.application.profile = str(path)
                return check_parameterized(original_circuit, compiled_circuit, configuration, **kwargs)

    # Circuit is not parameterized. Execute regular check
    if kwargs:
        # create the equivalence checker from keyword arguments
        ecm = EquivalenceCheckingManager(original_circuit, compiled_circuit, **kwargs)
    else:
        if configuration is None:
            configuration = Configuration()

        # create the equivalence checker from configuration
        ecm = EquivalenceCheckingManager(original_circuit, compiled_circuit, configuration)

    ecm.set_application_scheme(scheme)

    with resources.as_file(ref) as path:
        ecm.set_gate_cost_profile(str(path))

        # execute the check
        ecm.run()

    # obtain the result
    return ecm.get_results()
