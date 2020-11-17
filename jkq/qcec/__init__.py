#
# This file is part of JKQ QCEC library which is released under the MIT license.
# See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
#
from pyqcec import Method, StimuliType, _verify
import typing as _typing
import os as _os

__all__ = ['verify', 'Method', 'StimuliType']
_Path = _typing.Union[str, bytes, _os.PathLike]


def verify(file1: _Path, file2: _Path,
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
           ) -> object:
    """Interface to the JKQ QCEC tool for verifying quantum circuits

    :param file1: Path to first file
    :type file1: _Path
    :param file2: Path to second file
    :type file2: _Path
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
    :rtype: _typing.Dict[str, _typing.Any]
    """
    result = _verify({
        "file1": file1,
        "file2": file2,
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
