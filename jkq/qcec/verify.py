#
# This file is part of JKQ QCEC library which is released under the MIT license.
# See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
#
import pickle
from pathlib import Path
from typing import Any, Dict
from .pyqcec import Method, StimuliType, ver


def verify(circ1, circ2,
           method: Method = Method.proportional,
           tolerance: float = 1e-13,
           nsims: int = 16,
           fidelity: float = 0.999,
           stimuliType: StimuliType = StimuliType.classical,
           csv: bool = False,
           statistics: bool = False,
           storeCEXinput: bool = False,
           storeCEXoutput: bool = False,
           swapGateFusion: bool = False,
           singleQubitGateFusion: bool = False,
           removeDiagonalGatesBeforeMeasure = False
           ) -> Dict[str, Any]:
    """Interface to the JKQ QCEC tool for verifying quantum circuits

    :param circ1: Path to first circuit file, path to Qiskit QuantumCircuit pickle, or Qiskit QuantumCircuit object
    :param circ2: Path to second circuit file, path to Qiskit QuantumCircuit pickle, or Qiskit QuantumCircuit object
    :param method: Equivalence checking method to use (reference | naive | *proportional* | lookahead | simulation | compilation flow)
    :type method: Method
    :param tolerance: Numerical tolerance used during computation
    :type tolerance: float
    :param nsims: Number of simulations to conduct (for simulation method)
    :type nsims: int
    :param fidelity: Fidelity limit for comparison (for simulation method)
    :type fidelity: float
    :param stimuliType: Type of stimuli to use (for simulation method: *classical* | localquantum | globalquantum)
    :type stimuliType: StimuliType
    :param csv: Create CSV string for result
    :type csv: bool
    :param statistics: Print statistics
    :type statistics: bool
    :param storeCEXinput: Store counterexample input state vector (for simulation method)
    :type storeCEXinput: bool
    :param storeCEXoutput: Store resulting counterexample state vectors (for simulation method)
    :type storeCEXoutput: bool
    :param swapGateFusion: Optimization pass reconstructing SWAP operations
    :type swapGateFusion: bool
    :param singleQubitGateFusion: Optimization pass fusing consecutive single qubit gates
    :type singleQubitGateFusion: bool
    :param removeDiagonalGatesBeforeMeasure: Optimization pass removing diagonal gates before measurements
    :type removeDiagonalGatesBeforeMeasure: bool
    :return: JSON object containing results
    :rtype: Dict[str, Any]
    """

    if type(circ1) == str and Path(circ1).suffix == '.pickle':
        circ1 = pickle.load(open(circ1, "rb"))

    if type(circ2) == str and Path(circ2).suffix == '.pickle':
        circ2 = pickle.load(open(circ2, "rb"))

    result = ver(circ1, circ2,
                 {
                    "method": method.name,
                    "tolerance": tolerance,
                    "nsims": nsims,
                    "fidelity": fidelity,
                    "stimuliType": stimuliType.name,
                    "csv": csv,
                    "statistics": statistics,
                    "storeCEXinput": storeCEXinput,
                    "storeCEXoutput": storeCEXoutput,
                    "swapGateFusion": swapGateFusion,
                    "singleQubitGateFusion": singleQubitGateFusion,
                    "removeDiagonalGatesBeforeMeasure": removeDiagonalGatesBeforeMeasure
                 })

    if "error" in result:
        print(result["error"])

    return result
