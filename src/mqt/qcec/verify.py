"""The main entry point for the QCEC package."""

from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from qiskit import QuantumCircuit

    from ._compat.typing import Unpack
    from .configuration import ConfigurationOptions

from . import Configuration, EquivalenceCheckingManager
from .configuration import augment_config_from_kwargs
from .parameterized import __is_parameterized, check_parameterized


def verify(
    circ1: QuantumCircuit | str,
    circ2: QuantumCircuit | str,
    configuration: Configuration | None = None,
    **kwargs: Unpack[ConfigurationOptions],
) -> EquivalenceCheckingManager.Results:
    """Verify that ``circ1`` and ``circ2`` are equivalent.

    Wraps creating an instance of :class:`EquivalenceCheckingManager <.EquivalenceCheckingManager>`,
    calling :meth:`EquivalenceCheckingManager.run`,
    and calling :meth:`EquivalenceCheckingManager.get_results`.

    There are two (non-exclusive) ways of configuring the equivalence checking process:

    1. Pass a :class:`Configuration <.Configuration>` instance as the ``configuration`` argument.

    2. Pass keyword arguments to this function. These are directly incorporated into the :class:`Configuration <.Configuration>`.
    Any existing configuration is overridden by keyword arguments.

    Args:
        circ1: The first circuit.
        circ2: The second circuit.
        configuration: The configuration to use for the equivalence checking process.
        **kwargs: Keyword arguments to configure the equivalence checking process.

    Returns:
        The results of the equivalence checking process.
    """
    if configuration is None:
        configuration = Configuration()

    # prepare the configuration
    augment_config_from_kwargs(configuration, kwargs)

    if __is_parameterized(circ1) or __is_parameterized(circ2):
        return check_parameterized(circ1, circ2, configuration)

    # create the equivalence checker from configuration
    ecm = EquivalenceCheckingManager(circ1, circ2, configuration)

    # execute the check
    ecm.run()

    # obtain the result
    return ecm.get_results()
