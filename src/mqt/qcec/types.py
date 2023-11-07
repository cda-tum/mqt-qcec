"""Types for the QCEC module."""

from __future__ import annotations

from typing import Literal

ApplicationSchemeName = Literal["sequential", "one_to_one", "proportional", "lookahead", "gate_cost"]
EquivalenceCriterionName = Literal[
    "no_information",
    "not_equivalent",
    "equivalent",
    "equivalent_up_to_phase",
    "equivalent_up_to_global_phase",
    "probably_equivalent",
    "probably_not_equivalent",
]
StateTypeName = Literal["computational_basis", "random_1Q_basis", "stabilizer"]
