from __future__ import annotations

import random
from enum import Enum
from pathlib import Path
from typing import Any

import numpy as np
from qiskit import QuantumCircuit, transpile


class AncillaMode(str, Enum):
    """Enum for the ancilla mode."""

    NO_ANCILLA = "noancilla"
    RECURSION = "recursion"
    V_CHAIN = "v-chain"


single_qubit_gates_no_params = {
    "qubits": 1,
    "params": 0,
    "controls": 0,
    "gates": ["x", "y", "z", "h", "s", "sdg", "t", "tdg", "sx", "sxdg"],
}
single_qubit_gates_one_param = {
    "qubits": 1,
    "params": 1,
    "controls": 0,
    "gates": ["p", "rx", "ry", "rz"],
}
single_qubit_gates_three_params = {
    "qubits": 1,
    "params": 3,
    "controls": 0,
    "gates": ["u"],
}

two_qubit_gates = {"qubits": 2, "params": 0, "controls": 0, "gates": ["swap", "iswap"]}

single_controlled_single_qubit_gates_no_params = {
    "qubits": 1,
    "params": 0,
    "controls": 1,
    "gates": ["x", "y", "z", "h", "sx"],
}
single_controlled_single_qubit_gates_one_param = {
    "qubits": 1,
    "params": 1,
    "controls": 1,
    "gates": ["p", "rx", "ry", "rz"],
}
single_controlled_single_qubit_gates_three_params = {
    "qubits": 1,
    "params": 4,
    "controls": 1,
    "gates": ["u"],
}

single_controlled_two_qubit_gates = {
    "qubits": 2,
    "params": 0,
    "controls": 1,
    "gates": ["swap"],
}

double_controlled_single_qubit_gates_no_params = {
    "qubits": 1,
    "params": 0,
    "controls": 2,
    "gates": ["x"],
}

max_controls = 11
control_range = range(2, max_controls + 1)
mcx_no_ancilla = {
    "qubits": 1,
    "params": 0,
    "controls": control_range,
    "mode": AncillaMode.NO_ANCILLA,
    "ancilla_qubits": 0,
    "gates": ["x"],
}
mcx_recursion = {
    "qubits": 1,
    "params": 0,
    "controls": control_range,
    "mode": AncillaMode.RECURSION,
    "ancilla_qubits": 1,
    "gates": ["x"],
}
mcx_v_chain = {
    "qubits": 1,
    "params": 0,
    "controls": control_range,
    "mode": AncillaMode.V_CHAIN,
    "ancilla_qubits": None,
    "gates": ["x"],
}
mcphase = {
    "qubits": 1,
    "params": 1,
    "controls": control_range,
    "mode": None,
    "ancilla_qubits": 0,
    "gates": ["p"],
}

general_gates = [
    single_qubit_gates_no_params,
    single_qubit_gates_one_param,
    single_qubit_gates_three_params,
    two_qubit_gates,
    single_controlled_single_qubit_gates_no_params,
    single_controlled_single_qubit_gates_one_param,
    single_controlled_single_qubit_gates_three_params,
    single_controlled_two_qubit_gates,
    double_controlled_single_qubit_gates_no_params,
]

multi_controlled_gates = [mcphase]
multi_controlled_gates_no_ancilla = [mcx_no_ancilla]
multi_controlled_gates_recursion = [mcx_recursion]
multi_controlled_gates_v_chain = [mcx_v_chain]


def create_general_gate(qubits: int, params: int, controls: int, identifier: str) -> QuantumCircuit:
    """
    Create a ``QuantumCircuit`` containing a single gate ``identifier``
    with the given number of ``qubits``, ``params``, and ``controls``.
    """
    required_qubits = qubits + controls
    qc = QuantumCircuit(required_qubits)
    gate_identifier = "c" * controls + identifier

    parameter_list = []
    for _ in range(params):
        parameter_list.append(random.uniform(-np.pi, np.pi))

    getattr(qc, gate_identifier)(*parameter_list, *range(required_qubits))
    return qc


def create_multi_controlled_gate(
    qubits: int,
    params: int,
    controls: int,
    mode: AncillaMode | None,
    ancilla_qubits: int | None,
    identifier: str,
) -> QuantumCircuit:
    """
    Create a ``QuantumCircuit`` containing a single multi-controlled gate ``identifier``
    with the given number of ``qubits``, ``params``, and ``controls``
    using ``ancilla_qubits`` ancilla qubits and the given ancilla ``mode``.
    """

    required_qubits = qubits + controls

    # special handling for v-chain mode which is indicated by the ancilla_qubits being None
    if ancilla_qubits is None:
        ancilla_qubits = max(0, controls - 2)

    # special handling for recursion mode with less than 5 controls,
    # which does not require ancilla qubits
    if mode == "recursion" and controls < 5:
        ancilla_qubits = 0

    required_qubits += ancilla_qubits
    qc = QuantumCircuit(required_qubits)
    gate_identifier = "mc" + identifier

    parameter_list = []
    for _ in range(params):
        parameter_list.append(random.uniform(-np.pi, np.pi))

    if mode is not None:
        getattr(qc, gate_identifier)(
            *parameter_list,
            control_qubits=list(range(controls)),
            target_qubit=controls,
            ancilla_qubits=list(range(controls + 1, controls + 1 + ancilla_qubits)),
            mode=mode,
        )
    else:
        getattr(qc, gate_identifier)(*parameter_list, control_qubits=list(range(controls)), target_qubit=controls)
    return qc


def compute_cost(
    qc: QuantumCircuit,
    basis_gates: list[str],
    optimization_level: int = 1,
) -> int:
    """
    Compute the cost of a circuit by transpiling the circuit
    to a given ``basis_gates`` gate set and a certain ``optimization_level``.
    """
    transpiled_circuit = transpile(qc, basis_gates=basis_gates, optimization_level=optimization_level)
    return transpiled_circuit.size()


class GateType(Enum):
    """Enum for gate types."""

    GENERAL = 1
    MULTI_CONTROLLED = 2


def create_gate_profile_data(
    gate_collection: list[dict[str, Any]],
    gate_type: GateType,
    basis_gates: list[str] | None = None,
    optimization_level: int = 1,
) -> dict[tuple[str, int], int]:
    """
    Create a dictionary of gate profile data.
    """

    if basis_gates is None:
        basis_gates = ["id", "rz", "sx", "x", "cx"]

    profile_data = {}
    for gate_set in gate_collection:
        gates = gate_set["gates"]
        qubits = gate_set["qubits"]
        params = gate_set["params"]
        controls = gate_set["controls"]

        # pack single control numbers into list
        if gate_type == GateType.GENERAL:
            controls = [controls]

        for gate in gates:
            for control in controls:
                qc = None
                # create the gate
                if gate_type == GateType.GENERAL:
                    qc = create_general_gate(qubits, params, control, gate)
                elif gate_type == GateType.MULTI_CONTROLLED:
                    qc = create_multi_controlled_gate(
                        qubits,
                        params,
                        control,
                        gate_set["mode"],
                        gate_set["ancilla_qubits"],
                        gate,
                    )
                # compute the cost
                cost = compute_cost(qc, basis_gates, optimization_level)

                # add the cost to the profile data
                profile_data[(gate, control)] = cost
    return profile_data


def add_special_case_data(
    profile_data: dict[tuple[str, int], int],
    special_cases: dict[str, Any] | None = None,
) -> None:
    """
    Add special case data to the profile data. This is used to extrapolate the cost
    of specific rotation gates (e.g., S, T, ...) from the cost of the generic phase gate.
    """

    if special_cases is None:
        special_cases = {
            "gates": ["z", "s", "sdg", "t", "tdg"],
            "underlying_gate": "p",
        }

    gates = special_cases["gates"]
    underlying_gate = special_cases["underlying_gate"]

    for (g, nc), cost in profile_data.copy().items():
        if g is underlying_gate:
            for gate in gates:
                profile_data.setdefault((gate, nc), cost)


def generate_profile_name(optimization_level: int = 1, mode: AncillaMode = AncillaMode.NO_ANCILLA) -> str:
    """Generate a profile name based on the given optimization level and ancilla mode."""
    return "qiskit_O" + str(optimization_level) + "_" + mode + ".profile"


def write_profile_data_to_file(profile_data: dict[tuple[str, int], int], filename: Path) -> None:
    """Write the profile data to a file."""
    with Path(filename).open("w+", encoding="utf-8") as f:
        for (gate, controls), cost in profile_data.items():
            f.write(f"{gate} {controls} {cost}\n")


def check_recurrence(seq: list[int], order: int = 2) -> list[int] | None:
    """
    Tries to determine a recurrence relation with a given ``order`` in ``sequence``
    and returns the corresponding coefficients or ``None`` if no relation was determined.
    """
    if len(seq) < (2 * order + 1):
        return None

    mat, f = [], []
    for i in range(order):
        mat.append(seq[i : i + order])
        f.append(seq[i + order])

    if np.linalg.det(mat) == 0:
        return None

    coeffs = np.linalg.inv(mat).dot(f)

    for i in range(2 * order, len(seq)):
        predict = np.sum(coeffs * np.array(seq[i - order : i]))
        if abs(predict - seq[i]) > 10 ** (-2):
            return None

    return list(coeffs)


def find_continuation(
    profile_data: dict[tuple[str, int], int],
    gate: str,
    cutoff: int = 5,
    max_order: int = 3,
    max_control: int = 11,
    max_qubits: int = 128,
    prediction_cutoff: float = 1e6,
) -> None:
    """Tries to extrapolate from the given profile data by finding recurrence relations."""
    sequence = []
    for (g, _), cost in profile_data.items():
        if g is gate:
            sequence.append(cost)

    # sort the sequence
    sequence = sorted(sequence)

    # cut off the sequence at the cutoff point
    sequence = sequence[cutoff:]

    coeffs = None
    for order in range(1, max_order + 1):
        coeffs = check_recurrence(sequence, order)
        if coeffs is not None:
            break

    # if no recurrence sequence was found, assume the sequence is linear
    if coeffs is None:
        coeffs = [-1, 2]

    entries_to_compute = max_qubits - max_control - 1
    order = len(coeffs)
    for i in range(entries_to_compute):
        next_term = int(np.round(np.sum(coeffs * np.array(sequence[-order:]))))
        if next_term >= prediction_cutoff:
            break
        sequence.append(next_term)
        profile_data[(gate, max_control + i + 1)] = next_term


gate_collection_for_mode = {
    AncillaMode.NO_ANCILLA: multi_controlled_gates_no_ancilla,
    AncillaMode.RECURSION: multi_controlled_gates_recursion,
    AncillaMode.V_CHAIN: multi_controlled_gates_v_chain,
}
default_profile_path = Path(__file__).resolve().parent.joinpath("profiles")


def generate_profile(
    optimization_level: int = 1,
    mode: AncillaMode = AncillaMode.NO_ANCILLA,
    filepath: Path = None,
) -> None:
    """
    Generate a compilation flow profile for the given optimization level and ancilla mode.

    :param optimization_level: The IBM Qiskit optimization level to use for the profile (0, 1, 2, or 3).
    :type optimization_level: int
    :param mode: The `ancilla mode <.AncillaMode>` used for realizing multi-controlled Toffoli gates, as available in Qiskit.
    :type mode: AncillaMode
    :param filepath: The path to the directory where the profile should be stored. Defaults to the ``profiles`` directory in the ``mqt.qcec`` package.
    :type filepath: Path
    """
    if filepath is None:
        filepath = default_profile_path

    # generate general profile data
    profile = create_gate_profile_data(general_gates, GateType.GENERAL, optimization_level=optimization_level)

    # add multi-controlled gates
    profile.update(
        create_gate_profile_data(
            multi_controlled_gates,
            GateType.MULTI_CONTROLLED,
            optimization_level=optimization_level,
        )
    )
    find_continuation(profile, gate="p", max_control=max_controls)

    gate_collection = gate_collection_for_mode[mode]

    # add multi-controlled gates with specific mode
    profile.update(
        create_gate_profile_data(
            gate_collection,
            GateType.MULTI_CONTROLLED,
            optimization_level=optimization_level,
        )
    )
    find_continuation(profile, gate="x", max_control=max_controls)

    # add special case data
    add_special_case_data(profile)

    # write profile data to file
    filename = generate_profile_name(optimization_level=optimization_level, mode=mode)
    filepath = filepath.joinpath(filename)
    write_profile_data_to_file(profile, filepath)
    print(f"Wrote profile data to {filepath}")
