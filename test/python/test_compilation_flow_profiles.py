from __future__ import annotations

import filecmp
import sys
from pathlib import Path
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING or sys.version_info < (3, 9, 0):
    import importlib_resources as resources
else:
    from importlib import resources

import pytest
from mqt import qcec
from mqt.qcec.compilation_flow_profiles import generate_profile_name


@pytest.fixture(params=[0, 1, 2, 3])
def optimization_level(request: Any) -> int:
    return request.param


@pytest.fixture(params=[qcec.AncillaMode.NO_ANCILLA, qcec.AncillaMode.RECURSION, qcec.AncillaMode.V_CHAIN])
def ancilla_mode(request: Any) -> qcec.AncillaMode:
    return request.param


@pytest.mark.skipif(
    sys.version_info < (3, 11, 0) or sys.platform != "linux",
    reason="Since this check takes quite some time, it is only executed if the current platform is Linux and the Python version is 3.11 or higher.",
)
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
