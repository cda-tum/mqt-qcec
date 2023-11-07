"""Verify compilation flow results."""

from __future__ import annotations

import sys
import warnings
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing_extensions import Unpack

    from .configuration import ConfigurationOptions

if TYPE_CHECKING or sys.version_info < (3, 10, 0):
    import importlib_resources as resources
else:
    from importlib import resources

from qiskit import QuantumCircuit
from qiskit.transpiler.passes import ContainsInstruction

from . import ApplicationScheme, Configuration, EquivalenceCheckingManager
from .compilation_flow_profiles import AncillaMode, generate_profile_name
from .configuration import augment_config_from_kwargs
from .verify import verify


def __check_if_circuit_contains_measurements(circuit: QuantumCircuit) -> None:
    """Check if the circuit contains measurements and emits a warning if it does not.

    Args:
        circuit: The circuit to check.
    """
    analysis_pass = ContainsInstruction("measure")
    analysis_pass(circuit)
    if not analysis_pass.property_set["contains_measure"]:
        warnings.warn(
            UserWarning(
                "One of the circuits does not contain any measurements. "
                "This may lead to unexpected results since the measurements are used "
                "to infer the output qubit permutation at the end of the circuit. "
                "Please consider adding measurements to the circuit _before_ compilation."
            ),
            stacklevel=2,
        )


def verify_compilation(
    original_circuit: QuantumCircuit | str,
    compiled_circuit: QuantumCircuit | str,
    optimization_level: int = 1,
    ancilla_mode: AncillaMode = AncillaMode.NO_ANCILLA,
    configuration: Configuration | None = None,
    **kwargs: Unpack[ConfigurationOptions],
) -> EquivalenceCheckingManager.Results:
    """Verify compilation flow results.

    Similar to :func:`verify <.verify>`, but uses a dedicated compilation flow profile to guide the equivalence checking process.
    The compilation flow profile is determined by the ``optimization_level`` and ``ancilla_mode`` arguments.

    There are two (non-exclusive) ways of configuring the equivalence checking process:

    1. Pass a :class:`Configuration <.Configuration>` instance as the ``configuration`` argument.

    2. Pass keyword arguments to this function. These are directly incorporated into the :class:`Configuration <.Configuration>`.
    Any existing configuration is overridden by keyword arguments.

    Args:
        original_circuit: The original circuit.
        compiled_circuit: The compiled circuit.
        optimization_level: The optimization level used for compiling the circuit (0, 1, 2, or 3). Defaults to 1.
        ancilla_mode:
            The `ancilla mode <.AncillaMode>` used for realizing multi-controlled Toffoli gates, as available in Qiskit.
            Defaults to :attr:`.AncillaMode.NO_ANCILLA`.
        configuration: The configuration to use for the equivalence checking process.
        **kwargs: Keyword arguments to configure the equivalence checking process.

    Returns:
        The results of the equivalence checking process.
    """
    if configuration is None:
        configuration = Configuration()

    # prepare the configuration
    augment_config_from_kwargs(configuration, kwargs)

    if isinstance(original_circuit, QuantumCircuit):
        __check_if_circuit_contains_measurements(original_circuit)

    if isinstance(compiled_circuit, QuantumCircuit):
        __check_if_circuit_contains_measurements(compiled_circuit)

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
