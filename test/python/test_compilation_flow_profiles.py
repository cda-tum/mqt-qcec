"""Test compilation flow profiles."""

from __future__ import annotations

import filecmp
import sys
from pathlib import Path
from typing import TYPE_CHECKING, Any, cast

if TYPE_CHECKING or sys.version_info < (3, 9, 0):
    import importlib_resources as resources
else:
    from importlib import resources

import difflib

import pytest

from mqt import qcec
from mqt.qcec.compilation_flow_profiles import generate_profile_name


@pytest.fixture(params=[0, 1, 2, 3])
def optimization_level(request: Any) -> int:
    """Fixture for optimization levels."""
    return cast(int, request.param)


@pytest.fixture(params=[qcec.AncillaMode.NO_ANCILLA, qcec.AncillaMode.RECURSION, qcec.AncillaMode.V_CHAIN])
def ancilla_mode(request: Any) -> qcec.AncillaMode:
    """Fixture for ancilla modes."""
    return cast(qcec.AncillaMode, request.param)


def test_ancilla_mode_conversion(ancilla_mode: qcec.AncillaMode) -> None:
    """Test conversion and equality of ancilla modes."""
    ancilla_str = str(ancilla_mode)
    assert qcec.AncillaMode(ancilla_str) == ancilla_mode
    assert ancilla_str == ancilla_mode
    assert ancilla_mode == ancilla_str


@pytest.mark.skipif(
    sys.version_info < (3, 11, 0) or sys.platform != "linux",
    reason="Since this check takes quite some time, it is only executed if the current platform is Linux and the Python version is 3.11 or higher.",
)
def test_generated_profiles_are_still_valid(optimization_level: int, ancilla_mode: qcec.AncillaMode) -> None:
    """Test validity of generated profiles.

    The main intention of this check is to catch cases where an update in Qiskit changes the respective costs.
    """
    # generate the profile
    qcec.generate_profile(optimization_level=optimization_level, mode=ancilla_mode, filepath=Path())

    # get path to the profile from the package resources
    profile_name = generate_profile_name(optimization_level=optimization_level, mode=ancilla_mode)
    ref = resources.files("mqt.qcec") / "profiles" / profile_name

    # compare the generated profile with the reference profile
    with resources.as_file(ref) as path:
        equal = filecmp.cmp(path, profile_name, shallow=False)

        if equal:
            return

        ref_profile = path.read_text().splitlines(keepends=True)
        gen_profile = Path(profile_name).read_text().splitlines(keepends=True)
        diff = difflib.unified_diff(ref_profile, gen_profile, fromfile="reference", tofile="generated", n=0)
        num_diffs = sum(
            1
            for line in diff
            if (
                (line.startswith("-") and not line.startswith("---"))
                or (line.startswith("+") and not line.startswith("+++"))
            )
        )
        sys.stdout.writelines(diff)

        assert num_diffs <= 1, (
            f"The generated profile {profile_name} differs from the reference profile {ref} by {num_diffs} lines. "
            f"This might be due to a change in Qiskit. If this is the case, the reference profile should be updated."
        )
