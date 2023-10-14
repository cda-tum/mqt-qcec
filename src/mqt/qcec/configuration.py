"""Configuration options for the equivalence checking manager."""

from __future__ import annotations

from typing import TYPE_CHECKING, TypedDict

if TYPE_CHECKING:
    from . import ApplicationScheme, Configuration, StateType
    from .types import ApplicationSchemeName, StateTypeName


class ConfigurationOptions(TypedDict, total=False):
    """A dictionary of configuration options.

    The keys of this dictionary are the names of the configuration options.
    The values are the values of the configuration options.
    """

    # Application
    alternating_scheme: ApplicationScheme | ApplicationSchemeName
    construction_scheme: ApplicationScheme | ApplicationSchemeName
    profile: str
    simulation_scheme: ApplicationScheme | ApplicationSchemeName
    # Execution
    nthreads: int
    numerical_tolerance: float
    parallel: bool
    run_alternating_checker: bool
    run_construction_checker: bool
    run_simulation_checker: bool
    run_zx_checker: bool
    timeout: float
    # Functionality
    trace_threshold: float
    # Optimizations
    fix_output_permutation_mismatch: bool
    fuse_single_qubit_gates: bool
    reconstruct_swaps: bool
    remove_diagonal_gates_before_measure: bool
    reorder_operations: bool
    transform_dynamic_circuit: bool
    # Parameterized
    additional_instantiations: int
    parameterized_tolerance: float
    # Simulation
    fidelity_threshold: float
    max_sims: int
    seed: int
    state_type: StateType | StateTypeName
    store_cex_input: bool
    store_cex_output: bool


def augment_config_from_kwargs(config: Configuration, kwargs: ConfigurationOptions) -> None:
    """Augment an existing :class:`~mqt.qcec.Configuration` with options from a collection of keyword arguments.

    Args:
        config: The configuration to augment.
        kwargs: The arguments to build the configuration from.
    """
    for key, value in kwargs.items():
        if hasattr(config.application, key):
            setattr(config.application, key, value)
        elif hasattr(config.execution, key):
            setattr(config.execution, key, value)
        elif hasattr(config.functionality, key):
            setattr(config.functionality, key, value)
        elif hasattr(config.optimizations, key):
            setattr(config.optimizations, key, value)
        elif hasattr(config.parameterized, key):
            setattr(config.parameterized, key, value)
        elif hasattr(config.simulation, key):
            setattr(config.simulation, key, value)
        else:  # pragma: no cover
            msg = f"Invalid keyword argument: {key}"
            raise ValueError(msg)
