"""Regression test."""

from __future__ import annotations

from qiskit import QuantumCircuit, transpile
from qiskit.providers.fake_provider import FakeLondon

from mqt import qcec


def test_failure() -> None:
    """Test failure."""
    circ = QuantumCircuit(3)
    circ.x(2)
    circ.h(range(3))
    circ.ccx(0, 1, 2)
    circ.h(range(2))
    circ.x(range(2))
    circ.h(1)
    circ.cx(0, 1)
    circ.h(1)
    circ.x(range(2))
    circ.h(range(2))
    circ.measure_all()
    print("\n", circ.draw(fold=-1), sep="")

    optimization_level = 1
    circ_comp = transpile(circ, backend=FakeLondon(), optimization_level=optimization_level)
    print(circ_comp.draw(fold=-1))

    res = qcec.verify_compilation(
        circ,
        circ_comp,
        optimization_level=optimization_level,
        run_zx_checker=False,
        run_alternating_checker=True,
        run_simulation_checker=False,
    )
    print(f"Circuits are {res.equivalence}")
    assert res.considered_equivalent()
