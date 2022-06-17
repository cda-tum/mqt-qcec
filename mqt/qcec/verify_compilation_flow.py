import pkg_resources
from pathlib import Path
from mqt.qcec import EquivalenceCheckingManager, Configuration
from mqt.qcec.compilation_flow_profiles import AncillaMode, generate_profile_name


def verify_compilation(original_circuit, compiled_circuit,
                       optimization_level: int = 1, ancilla_mode: AncillaMode = AncillaMode.NO_ANCILLA,
                       configuration: Configuration = Configuration()) -> EquivalenceCheckingManager.Results:
    # create the equivalence checker
    ecm = EquivalenceCheckingManager(original_circuit, compiled_circuit, configuration)

    # use the gate_cost scheme for the verification
    ecm.set_application_scheme("gate_cost")

    # get the pre-defined profile for the gate_cost scheme
    profile = pkg_resources.resource_filename(__name__, 'profiles/' + generate_profile_name(optimization_level=optimization_level, mode=ancilla_mode))
    ecm.set_gate_cost_profile(str(Path(__file__).resolve().parent.joinpath(profile)))

    # execute the check
    ecm.run()

    # obtain the result
    return ecm.get_results()
