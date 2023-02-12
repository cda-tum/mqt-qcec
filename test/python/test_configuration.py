"""Test the configuration of the equivalence checker."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

if TYPE_CHECKING:
    from mqt.qcec.types import ApplicationSchemeName, StateTypeName

from mqt import qcec


@pytest.mark.parametrize(
    ("application_scheme_string", "application_scheme_enum"),
    [
        ("sequential", qcec.ApplicationScheme.sequential),
        ("reference", qcec.ApplicationScheme.sequential),
        ("one_to_one", qcec.ApplicationScheme.one_to_one),
        ("naive", qcec.ApplicationScheme.one_to_one),
        ("lookahead", qcec.ApplicationScheme.lookahead),
        ("gate_cost", qcec.ApplicationScheme.gate_cost),
        ("compilation_flow", qcec.ApplicationScheme.gate_cost),
        ("proportional", qcec.ApplicationScheme.proportional),
    ],
)
def test_application_scheme(
    application_scheme_string: ApplicationSchemeName, application_scheme_enum: qcec.ApplicationScheme
) -> None:
    """Test the application scheme enum."""
    assert qcec.ApplicationScheme(application_scheme_string) == application_scheme_enum

    config = qcec.Configuration()

    config.application.construction_scheme = application_scheme_enum
    config.application.simulation_scheme = application_scheme_enum
    config.application.alternating_scheme = application_scheme_enum


def test_timeout() -> None:
    """Test the timeout configuration."""
    config = qcec.Configuration()
    config.execution.timeout = 60
    config.execution.timeout = 60.0


@pytest.mark.parametrize(
    ("state_type_string", "state_type_enum"),
    [
        ("computational_basis", qcec.StateType.computational_basis),
        ("classical", qcec.StateType.computational_basis),
        ("random_1Q_basis", qcec.StateType.random_1Q_basis),
        ("local_quantum", qcec.StateType.random_1Q_basis),
        ("stabilizer", qcec.StateType.stabilizer),
        ("global_quantum", qcec.StateType.stabilizer),
    ],
)
def test_state_type(state_type_string: StateTypeName, state_type_enum: qcec.StateType) -> None:
    """Test the state type enum."""
    assert qcec.StateType(state_type_string) == state_type_enum

    config = qcec.Configuration()
    config.simulation.state_type = state_type_enum
