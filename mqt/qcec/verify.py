from __future__ import annotations

from typing import Any

from mqt.qcec import Configuration, EquivalenceCheckingManager
from qiskit import QuantumCircuit


def verify(
    circ1: QuantumCircuit | str, circ2: QuantumCircuit | str, configuration: Configuration | None = None, **kwargs: Any
) -> EquivalenceCheckingManager.Results:
    """
    Verify that ``circ1`` and ``circ2`` are equivalent.
    Wraps creating an instance of :class:`EquivalenceCheckingManager <.EquivalenceCheckingManager>`,
    calling :meth:`EquivalenceCheckingManager.run`,
    and calling :meth:`EquivalenceCheckingManager.get_result`.
    If ``configuration`` is not ``None``, it is used to configure the ``EquivalenceCheckingManager``.
    """

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
