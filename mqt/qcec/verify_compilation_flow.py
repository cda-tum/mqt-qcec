from __future__ import annotations

import sys
from typing import TYPE_CHECKING

if TYPE_CHECKING:  # pragma: no cover
    from qiskit import QuantumCircuit
    from typing_extensions import Unpack

    from mqt.qcec.configuration import ConfigurationOptions

if sys.version_info < (3, 10, 0):
    import importlib_resources as resources
else:
    from importlib import resources  # type: ignore[no-redef]

from mqt.qcec import ApplicationScheme, Configuration, EquivalenceCheckingManager
from mqt.qcec.compilation_flow_profiles import AncillaMode, generate_profile_name
from mqt.qcec.configuration import augment_config_from_kwargs
from mqt.qcec.verify import verify


def verify_compilation(
    original_circuit: QuantumCircuit | str,
    compiled_circuit: QuantumCircuit | str,
    optimization_level: int = 1,
    ancilla_mode: AncillaMode = AncillaMode.NO_ANCILLA,
    configuration: Configuration | None = None,
    **kwargs: Unpack[ConfigurationOptions],
) -> EquivalenceCheckingManager.Results:
    """
    Similar to :func:`verify <.verify>`, but uses a dedicated compilation flow profile to guide the equivalence checking process.
    The compilation flow profile is determined by the ``optimization_level`` and ``ancilla_mode`` arguments.

    There are two (non-exclusive) ways of configuring the equivalence checking process:

    1. Pass a :class:`Configuration <.Configuration>` instance as the ``configuration`` argument.

    2. Pass keyword arguments to this function. These are directly incorporated into the :class:`Configuration <.Configuration>`.
    Any existing configuration is overridden by keyword arguments.

    :param original_circuit: The original circuit.
    :param compiled_circuit: The compiled circuit.
    :param optimization_level: The optimization level used for compiling the circuit (0, 1, 2, or 3).
    :param ancilla_mode: The `ancilla mode <.AncillaMode>` used for realizing multi-controlled Toffoli gates, as available in Qiskit
    :param configuration: The configuration to use for the equivalence checking process.
    :param kwargs: Keyword arguments to configure the equivalence checking process.
    :return: The results of the equivalence checking process.
    """
    if configuration is None:
        configuration = Configuration()

    # prepare the configuration
    augment_config_from_kwargs(configuration, kwargs)

    # use the gate_cost scheme for the verification
    configuration.application.construction_scheme = ApplicationScheme.gate_cost
    configuration.application.simulation_scheme = ApplicationScheme.gate_cost
    configuration.application.alternating_scheme = ApplicationScheme.gate_cost

    # get the pre-defined profile for the gate_cost scheme
    profile_name = generate_profile_name(optimization_level=optimization_level, mode=ancilla_mode)
    ref = resources.files("mqt.qcec") / "profiles" / profile_name
    with resources.as_file(ref) as path:
        configuration.application.profile = str(path)

    return verify(original_circuit, compiled_circuit, configuration=configuration)
