from __future__ import annotations

from typing import Any

from mqt.qcec import Configuration, EquivalenceCheckingManager
from qiskit import QuantumCircuit

from .symbolic import check_symbolic


def verify(
    circ1: QuantumCircuit | str,
    circ2: QuantumCircuit | str,
    configuration: Configuration | None = None,
    n_symbolic_checks: int = 6,
    symbolic_tol: float = 1e-12,
    **kwargs: Any,
) -> EquivalenceCheckingManager.Results:
    """
    Verify that ``circ1`` and ``circ2`` are equivalent.

    Wraps creating an instance of :class:`EquivalenceCheckingManager <.EquivalenceCheckingManager>`,
    calling :meth:`EquivalenceCheckingManager.run`,
    and calling :meth:`EquivalenceCheckingManager.get_result`.

    There are two ways of configuring the equivalence checking process:

    1. Pass a :class:`Configuration <.Configuration>` instance as the ``configuration`` argument.

    2. Pass keyword arguments to this function. These are directly passed to the :meth:`constructor <.EquivalenceCheckingManager.__init__>` of the :class:`EquivalenceCheckingManager`.

    :param circ1: The first circuit.
    :param circ2: The second circuit.
    :param configuration: The configuration to use for the equivalence checking process.
    :param n_symbolic_checks: The number of times a parametrized circuit should be instantiated and checked
    :param symbolic_tol: The tolerance to which instantiated parameters should be recognized as 0
    :param kwargs: Keyword arguments to pass to the :class:`EquivalenceCheckingManager <.EquivalenceCheckingManager>` constructor.
    :return: The results of the equivalence checking process.
    """
    if (not isinstance(circ1, str) and circ1.parameters) or (not isinstance(circ2, str) and circ2.parameters):
        return check_symbolic(circ1, circ2, n_symbolic_checks, symbolic_tol, **kwargs)

    if kwargs:
        # create the equivalence checker from keyword arguments
        ecm = EquivalenceCheckingManager(circ1, circ2, **kwargs)
    else:
        if configuration is None:
            configuration = Configuration()

        # create the equivalence checker from configuration
        ecm = EquivalenceCheckingManager(circ1, circ2, configuration)

    # execute the check
    ecm.run()

    # obtain the result
    return ecm.get_results()
