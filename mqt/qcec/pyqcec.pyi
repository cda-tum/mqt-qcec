from __future__ import annotations

import datetime
from typing import Any, ClassVar, overload

class ApplicationScheme:
    __members__: ClassVar[dict[ApplicationScheme, int]] = ...  # read-only
    gate_cost: ClassVar[ApplicationScheme] = ...
    lookahead: ClassVar[ApplicationScheme] = ...
    one_to_one: ClassVar[ApplicationScheme] = ...
    proportional: ClassVar[ApplicationScheme] = ...
    sequential: ClassVar[ApplicationScheme] = ...
    @overload
    def __init__(self, value: int) -> None: ...
    @overload
    def __init__(self, arg0: str) -> None: ...
    def name(self) -> str: ...
    def __eq__(self, other: object) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    def __str__(self) -> str: ...
    @property
    def value(self) -> int: ...

class Configuration:
    class Application:
        alternating_scheme: ApplicationScheme
        construction_scheme: ApplicationScheme
        profile: str
        simulation_scheme: ApplicationScheme
        def __init__(self) -> None: ...

    class Execution:
        nthreads: int
        numerical_tolerance: float
        parallel: bool
        run_alternating_checker: bool
        run_construction_checker: bool
        run_simulation_checker: bool
        run_zx_checker: bool
        timeout: datetime.timedelta
        def __init__(self) -> None: ...

    class Functionality:
        trace_threshold: float
        def __init__(self) -> None: ...

    class Optimizations:
        fix_output_permutation_mismatch: bool
        fuse_single_qubit_gates: bool
        reconstruct_swaps: bool
        remove_diagonal_gates_before_measure: bool
        reorder_operations: bool
        transform_dynamic_circuit: bool
        def __init__(self) -> None: ...

    class Parameterized:
        additional_instantiations: int
        parameterized_tolerance: float
        def __init__(self) -> None: ...

    class Simulation:
        fidelity_threshold: float
        max_sims: int
        seed: int
        state_type: StateType
        store_cex_input: bool
        store_cex_output: bool
        def __init__(self) -> None: ...
    application: Configuration.Application
    execution: Configuration.Execution
    functionality: Configuration.Functionality
    optimizations: Configuration.Optimizations
    parameterized: Configuration.Parameterized
    simulation: Configuration.Simulation
    def __init__(self) -> None: ...
    def json(self) -> dict[str, Any]: ...

class EquivalenceCheckingManager:
    class Results:
        cex_input: list[complex]
        cex_output1: list[complex]
        cex_output2: list[complex]
        check_time: float
        equivalence: EquivalenceCriterion
        performed_instantiations: int
        performed_simulations: int
        preprocessing_time: float
        started_simulations: int
        def __init__(self) -> None: ...
        def considered_equivalent(self) -> bool: ...
        def json(self) -> dict[str, Any]: ...
        def __repr__(self) -> str: ...

    @overload
    def __init__(
        self,
        circ1: object,
        circ2: object,
        numerical_tolerance: float = ...,
        parallel: bool = ...,
        nthreads: int = ...,
        timeout: datetime.timedelta | float = ...,
        run_construction_checker: bool = ...,
        run_simulation_checker: bool = ...,
        run_alternating_checker: bool = ...,
        run_zx_checker: bool = ...,
        fix_output_permutation_mismatch: bool = ...,
        fuse_single_qubit_gates: bool = ...,
        reconstruct_swaps: bool = ...,
        remove_diagonal_gates_before_measure: bool = ...,
        transform_dynamic_circuit: bool = ...,
        reorder_operations: bool = ...,
        construction_scheme: ApplicationScheme | str = ...,
        simulation_scheme: ApplicationScheme | str = ...,
        alternating_scheme: ApplicationScheme | str = ...,
        profile: str = ...,
        trace_threshold: float = ...,
        fidelity_threshold: float = ...,
        max_sims: int = ...,
        state_type: StateType | str = ...,
        seed: int = ...,
        store_cex_input: bool = ...,
        store_cex_output: bool = ...,
        parameterized_tolerance: float = ...,
        additional_instantiations: int = ...,
    ) -> None: ...
    @overload
    def __init__(self, circ1: object, circ2: object, config: Configuration) -> None: ...
    def disable_all_checkers(self) -> None: ...
    def equivalence(self) -> EquivalenceCriterion: ...
    def fix_output_permutation_mismatch(self) -> None: ...
    def fuse_single_qubit_gates(self) -> None: ...
    def get_configuration(self) -> Configuration: ...
    def get_results(self) -> EquivalenceCheckingManager.Results: ...
    def json(self) -> dict[str, Any]: ...
    def reconstruct_swaps(self) -> None: ...
    def reorder_operations(self) -> None: ...
    def run(self) -> None: ...
    def set_alternating_application_scheme(self, scheme: ApplicationScheme | str = ...) -> None: ...
    def set_alternating_checker(self, enable: bool = ...) -> None: ...
    def set_alternating_gate_cost_profile(self, profile: str = ...) -> None: ...
    def set_application_scheme(self, scheme: ApplicationScheme | str = ...) -> None: ...
    def set_construction_application_scheme(self, scheme: ApplicationScheme | str = ...) -> None: ...
    def set_construction_checker(self, enable: bool = ...) -> None: ...
    def set_construction_gate_cost_profile(self, profile: str = ...) -> None: ...
    def set_fidelity_threshold(self, threshold: float = ...) -> None: ...
    def set_gate_cost_profile(self, profile: str = ...) -> None: ...
    def set_max_sims(self, sims: int = ...) -> None: ...
    def set_nthreads(self, nthreads: int = ...) -> None: ...
    def set_parallel(self, enable: bool = ...) -> None: ...
    def set_seed(self, seed: int = ...) -> None: ...
    def set_simulation_application_scheme(self, scheme: ApplicationScheme | str = ...) -> None: ...
    def set_simulation_checker(self, enable: bool = ...) -> None: ...
    def set_simulation_gate_cost_profile(self, profile: str = ...) -> None: ...
    def set_state_type(self, type: StateType | str = ...) -> None: ...
    def set_timeout(self, timeout: datetime.timedelta | float = ...) -> None: ...
    def set_tolerance(self, tolerance: float = ...) -> None: ...
    def set_trace_threshold(self, threshold: float = ...) -> None: ...
    def set_zx_checker(self, enable: bool = ...) -> None: ...
    def store_cex_input(self, enable: bool = ...) -> None: ...
    def store_cex_output(self, enable: bool = ...) -> None: ...
    def __repr__(self) -> str: ...

class EquivalenceCriterion:
    __members__: ClassVar[dict[EquivalenceCriterion, int]] = ...  # read-only
    equivalent: ClassVar[EquivalenceCriterion] = ...
    equivalent_up_to_global_phase: ClassVar[EquivalenceCriterion] = ...
    equivalent_up_to_phase: ClassVar[EquivalenceCriterion] = ...
    no_information: ClassVar[EquivalenceCriterion] = ...
    not_equivalent: ClassVar[EquivalenceCriterion] = ...
    probably_equivalent: ClassVar[EquivalenceCriterion] = ...
    probably_not_equivalent: ClassVar[EquivalenceCriterion] = ...
    @overload
    def __init__(self, value: int) -> None: ...
    @overload
    def __init__(self, arg0: str) -> None: ...
    def name(self) -> str: ...
    def __eq__(self, other: object) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    def __str__(self) -> str: ...
    @property
    def value(self) -> int: ...

class StateType:
    __members__: ClassVar[dict[StateType, int]] = ...  # read-only
    computational_basis: ClassVar[StateType] = ...
    random_1Q_basis: ClassVar[StateType] = ...
    stabilizer: ClassVar[StateType] = ...
    @overload
    def __init__(self, value: int) -> None: ...
    @overload
    def __init__(self, arg0: str) -> None: ...
    def name(self) -> str: ...
    def __eq__(self, other: object) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    def __str__(self) -> str: ...
    @property
    def value(self) -> int: ...