from __future__ import annotations

import sys
from typing import Any

if sys.version_info < (3, 10, 0):
    import importlib_resources as resources
else:
    from importlib import resources

from mqt.qcec import Configuration, EquivalenceCheckingManager
from mqt.qcec.compilation_flow_profiles import AncillaMode, generate_profile_name
from qiskit import QuantumCircuit


def verify_compilation(
    original_circuit: QuantumCircuit | str,
    compiled_circuit: QuantumCircuit | str,
    optimization_level: int = 1,
    ancilla_mode: AncillaMode = AncillaMode.NO_ANCILLA,
    configuration: Configuration | None = None,
    **kwargs: Any,
) -> EquivalenceCheckingManager.Results:
    """
    Verify that the ``compiled_circuit`` (compiled with a certain ``optimization_level`` amd ``ancilla_mode``) is equivalent to the ``original_circuit``.
    If ``configuration`` is not ``None``, it is used to configure the ``EquivalenceCheckingManager``.
    """

    if kwargs:
        # create the equivalence checker from keyword arguments
        ecm = EquivalenceCheckingManager(original_circuit, compiled_circuit, **kwargs)
    else:
        if configuration is None:
            configuration = Configuration()

        # create the equivalence checker from configuration
        ecm = EquivalenceCheckingManager(original_circuit, compiled_circuit, configuration)

    # use the gate_cost scheme for the verification
    ecm.set_application_scheme("gate_cost")

    # get the pre-defined profile for the gate_cost scheme
    profile_name = generate_profile_name(optimization_level=optimization_level, mode=ancilla_mode)
    ref = resources.files("mqt.qcec") / "profiles" / profile_name
    with resources.as_file(ref) as path:
        ecm.set_gate_cost_profile(str(path))

        # execute the check
        ecm.run()

    # obtain the result
    return ecm.get_results()
