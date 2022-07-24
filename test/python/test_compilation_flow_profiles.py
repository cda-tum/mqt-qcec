from __future__ import annotations

import filecmp
from pathlib import Path

try:
    from importlib import resources
except ImportError:
    import importlib_resources as resources

import pytest
from mqt import qcec
from mqt.qcec.compilation_flow_profiles import generate_profile_name


@pytest.fixture(params=[0, 1, 2, 3])
def optimization_level(request: pytest.request) -> int:
    return request.param


@pytest.fixture(params=[qcec.AncillaMode.NO_ANCILLA, qcec.AncillaMode.RECURSION, qcec.AncillaMode.V_CHAIN])
def ancilla_mode(request: pytest.request) -> qcec.AncillaMode:
    return request.param


def test_generated_profiles_are_still_valid(optimization_level: int, ancilla_mode: qcec.AncillaMode) -> None:
    """
    Test that the generated profiles are still valid.
    The main intention of this check is to catch cases where an update in Qiskit changes the respective costs.
    """

    # generate the profile
    qcec.generate_profile(optimization_level=optimization_level, mode=ancilla_mode, filepath=Path("."))

    # get path to the profile from the package resources
    profile_name = generate_profile_name(optimization_level=optimization_level, mode=ancilla_mode)
    ref = resources.files("mqt.qcec") / "profiles" / profile_name

    # compare the generated profile with the reference profile
    with resources.as_file(ref) as path:
        assert filecmp.cmp(path, profile_name)
