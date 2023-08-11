Installation
============

QCEC is mainly developed as a C++ library that builds upon `MQT Core <https://github.com/cda-tum/mqt-core>`_, which forms the backbone of the MQT.
In order to make the tool as accessible as possible, it comes with an easy-to-use Python interface.

We encourage installing QCEC via pip (preferably in a `virtual environment <https://docs.python.org/3/library/venv.html>`_):

    .. code-block:: console

        (venv) $ pip install mqt.qcec

In most practical cases (under 64-bit Linux, MacOS incl. Apple Silicon, and Windows), this requires no compilation and merely downloads and installs a platform-specific pre-built wheel.

.. note::
    In order to set up a virtual environment, you can use the following commands:

    .. code-block:: console

        $ python3 -m venv venv
        $ source venv/bin/activate

    If you are using Windows, you can use the following commands instead:

    .. code-block:: console

        $ python3 -m venv venv
        $ venv\Scripts\activate.bat

    It is recommended to make sure that you are using the latest version of pip, setuptools, and wheel before trying to install the project:

    .. code-block:: console

        (venv) $ pip install --upgrade pip setuptools wheel

.. warning::
    As of version 2.2.0, support for Python 3.7 has been officially dropped.
    We strongly recommend that users upgrade to a more recent version of Python to ensure compatibility and continue receiving updates and support.
    Thank you for your understanding.

A Detailed Walk Through
#######################

First, save the following lines as :code:`ghz_3.py` in a folder where you want to install QCEC and run the example:

    .. code-block:: python

        from qiskit import QuantumCircuit, transpile
        from qiskit.providers.fake_provider import FakeLondon
        from mqt import qcec

        # create your quantum circuit
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        circ.measure_all()
        print(circ.draw(fold=-1))

        # compile circuit to 5-qubit London Architecture
        circ_comp = transpile(circ, backend=FakeLondon())
        print(circ_comp.draw(fold=-1))

        # verify the equivalence of both circuits
        result = qcec.verify(circ, circ_comp)

        # print the result
        print(result.equivalence)

Then, the following snippet shows the installation process from setting up the virtual environment to running a small example program.

    .. code-block:: console

        $ python3 -m venv venv
        $ . venv/bin/activate
        (venv) $ pip install -U pip setuptools wheel
        (venv) $ pip install mqt.qcec
        (venv) $ python3 ghz_3.py
                ┌───┐           ░ ┌─┐
           q_0: ┤ H ├──■────■───░─┤M├──────
                └───┘┌─┴─┐  │   ░ └╥┘┌─┐
           q_1: ─────┤ X ├──┼───░──╫─┤M├───
                     └───┘┌─┴─┐ ░  ║ └╥┘┌─┐
           q_2: ──────────┤ X ├─░──╫──╫─┤M├
                          └───┘ ░  ║  ║ └╥┘
        meas: 3/═══════════════════╩══╩══╩═
                                   0  1  2
                                                            ░
        ancilla_0 -> 0 ─────────────────────────────────────░──────────
                                                      ┌───┐ ░       ┌─┐
              q_2 -> 1 ───────────────────────────────┤ X ├─░───────┤M├
                                                      └─┬─┘ ░       └╥┘
        ancilla_1 -> 2 ─────────────────────────────────┼───░────────╫─
                                  ┌───┐     ┌───┐       │   ░ ┌─┐    ║
              q_1 -> 3 ───────────┤ X ├──■──┤ X ├──■────■───░─┤M├────╫─
                       ┌─────────┐└─┬─┘┌─┴─┐└─┬─┘┌─┴─┐      ░ └╥┘┌─┐ ║
              q_0 -> 4 ┤ U2(0,π) ├──■──┤ X ├──■──┤ X ├──────░──╫─┤M├─╫─
                       └─────────┘     └───┘     └───┘      ░  ║ └╥┘ ║
               meas: 3/════════════════════════════════════════╩══╩══╩═
                                                               0  1  2
        equivalent


Building from Source for Performance
####################################

In order to get the best performance out of QCEC and enable platform-specific compiler optimizations that cannot be enabled on portable wheels, it is recommended to build the package from source via:

    .. code-block:: console

        (venv) $ pip install mqt.qcec --no-binary mqt.qcec

This requires a `C++ compiler <https://en.wikipedia.org/wiki/List_of_compilers#C++_compilers>`_ compiler supporting *C++17* and a minimum `CMake <https://cmake.org/>`_ version of *3.19*.

The library is continuously tested under Linux, MacOS, and Windows using the `latest available system versions for GitHub Actions <https://github.com/actions/virtual-environments>`_.
In order to access the latest build logs, visit `qcec/actions/workflows/ci.yml <https://github.com/cda-tum/qcec/actions/workflows/ci.yml>`_.

.. note::
    We noticed some issues when compiling with Microsoft's *MSCV* compiler toolchain. If you want to start development on this project under Windows, consider using the *clang* compiler toolchain. A detailed description of how to set this up can be found `here <https://docs.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-160>`_.
