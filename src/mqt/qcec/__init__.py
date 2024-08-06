"""MQT QCEC library.

This file is part of the MQT QCEC library released under the MIT license.
See README.md or go to https://github.com/cda-tum/qcec for more information.
"""

from __future__ import annotations

from ._version import version as __version__
from .pyqcec import (
    ApplicationScheme,
    Configuration,
    EquivalenceCheckingManager,
    EquivalenceCriterion,
    StateType,
)
from .verify import verify

# Conditionally import functionality depending on the availability of Qiskit
try:
    from .compilation_flow_profiles import AncillaMode, generate_profile
    from .verify_compilation_flow import verify_compilation

    qiskit_installed = True
except ImportError:
    qiskit_installed = False

__all__ = [
    "ApplicationScheme",
    "Configuration",
    "EquivalenceCheckingManager",
    "EquivalenceCriterion",
    "StateType",
    "__version__",
    "verify",
]
if qiskit_installed:
    __all__ += ["AncillaMode", "generate_profile", "verify_compilation"]
