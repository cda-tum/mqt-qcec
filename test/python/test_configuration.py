import pytest

from mqt import qcec


@pytest.mark.parametrize("application_scheme_string, application_scheme_enum", [
    ("sequential", qcec.ApplicationScheme.sequential),
    ("reference", qcec.ApplicationScheme.sequential),
    ("one_to_one", qcec.ApplicationScheme.one_to_one),
    ("naive", qcec.ApplicationScheme.one_to_one),
    ("lookahead", qcec.ApplicationScheme.lookahead),
    ("gate_cost", qcec.ApplicationScheme.gate_cost),
    ("compilation_flow", qcec.ApplicationScheme.gate_cost),
    ("proportional", qcec.ApplicationScheme.proportional),
])
def test_application_scheme(application_scheme_enum, application_scheme_string):
    assert qcec.ApplicationScheme(application_scheme_string) == application_scheme_enum

    config = qcec.Configuration()

    config.application.construction_scheme = application_scheme_enum
    config.application.construction_scheme = application_scheme_string

    config.application.simulation_scheme = application_scheme_enum
    config.application.simulation_scheme = application_scheme_string

    config.application.alternating_scheme = application_scheme_enum
    config.application.alternating_scheme = application_scheme_string


def test_timeout():
    import datetime

    config = qcec.Configuration()
    config.execution.timeout = 60.
    config.execution.timeout = datetime.timedelta(seconds=60)


@pytest.mark.parametrize("state_type_string, state_type_enum", [
    ("computational_basis", qcec.StateType.computational_basis),
    ("classical", qcec.StateType.computational_basis),
    ("random_1Q_basis", qcec.StateType.random_1Q_basis),
    ("local_quantum", qcec.StateType.random_1Q_basis),
    ("stabilizer", qcec.StateType.stabilizer),
    ("global_quantum", qcec.StateType.stabilizer),
])
def test_state_type(state_type_enum, state_type_string):
    assert qcec.StateType(state_type_string) == state_type_enum

    config = qcec.Configuration()
    config.simulation.state_type = state_type_enum
    config.simulation.state_type = state_type_string
