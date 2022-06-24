Installation for Developers
===========================

In order to start developing, clone the QCEC repository using

    .. code-block:: console

        $ git clone --recurse-submodules https://github.com/cda-tum/qcec

Note the :code:`--recurse-submodules` flag. It is required to also clone all the required submodules. If you happen to forget passing the flag on your initial clone, you can initialize all the submodules by executing :code:`git submodule update --init --recursive` in the main project directory.

A C++ compiler supporting *C++17* and a minimum CMake version of *3.14* is required to build the project.

Working on the core C++ library
###############################

Our projects use *CMake* as the main build configuration tool.
Building a project using CMake is a two-stage process.
First, CMake needs to be *configured* by calling

    .. code-block:: console

        $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

This tells CMake to search the current directory :code:`.` (passed via :code:`-S`) for a :code:`CMakeLists.txt` file and process it into a directory :code:`build` (passed via :code:`-B`).
The flag :code:`-DCMAKE_BUILD_TYPE=Release` tells CMake to configure a *Release* build (as opposed to, e.g., a *Debug* build).

After configuring with CMake, the project can be built by calling

    .. code-block:: console

        $ cmake --build build --config Release

This tries to build the project in the :code:`build` directory (passed via :code:`--build`).
Some operating systems and developer environments explicitly require a configuration to be set, which is why the :code:`--config` flag is also passed to the build command. The flag :code:`--parallel <NUMBER_OF_THREADS>` may be added to trigger a parallel build.

Building the project this way generates

- the main library :code:`libqcec.a` (Unix) / :code:`qcec.lib` (Windows) in the :code:`build/src` directory
- a test executable :code:`qcec_test` containing unit tests in the :code:`build/test` directory (this requires passing :code:`-DBUILD_QCEC_TESTS=ON` to CMake during configuration)
- the Python bindings library :code:`pyqcec.<...>` in the :code:`build/mqt/qcec` directory (this requires passing :code:`-DBINDINGS=ON` to CMake during configuration)

Working on the Python module
############################

The :code:`mqt.qcec` Python module can be conveniently built locally by calling

    .. code-block:: console

        (venv) $ pip install --editable .[dev]

The :code:`--editable` flag ensures that changes in the Python code are instantly available without re-running the command. The :code:`[dev]` extra makes sure that all dependencies for running the Python tests and building the documentation are available.

.. note::
    When using the :code:`zsh` shell it might be necessary to add double quotes around the :code:`.[dev]` part of the command.

`Pybind11 <https://pybind11.readthedocs.io/>`_ is used for providing bindings of the C++ core library to Python (see `bindings.cpp <https://github.com/cda-tum/qcec/tree/main/mqt/qcec/bindings.cpp>`_).
If parts of the C++ code have been changed, the above command has to be run again to make the changes visible in Python.

Running tests
#############

C++ core library
----------------

The C++ part of the code base is tested by unit tests using the `googletest <https://google.github.io/googletest/primer.html>`_ framework.
The corresponding test files can be found in the :code:`test` directory. In order to build the tests, CMake first has to be configured with the :code:`-DBUILD_QCEC_TESTS=ON` flag, i.e.,

    .. code-block:: console

        $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_QCEC_TESTS=ON

Then, the test executable :code:`qcec_test` is built in the :code:`build/test` directory by calling

    .. code-block:: console

        $ cmake --build build --config Release --target qcec_test

From there, the tests can be started by simply calling

    .. code-block:: console

        [.../build/test] $ ./qcec_test

Python interface and functionality
----------------------------------

The Python part of the code base is tested by unit tests using the `pytest <https://docs.pytest.org/en/latest/>`_ framework.
The corresponding test files can be found in the :code:`test/python` directory.
To start the tests, simply call

    .. code-block:: console

        (venv) $ python -m pytest ./test/python

.. note::
    If you haven't already installed the package with the :code:`[dev]` extra as demonstrated above, the necessary dependencies for running the Python tests can be installed by calling

        .. code-block:: console

            (venv) $ pip install --editable .[test]

Building the documentation
##########################

Building this documentation locally is as easy as calling

    .. code-block:: console

        (venv) [.../docs] $ make clean && make html

The resulting HTML documentation (:code:`index.html`) can be found in the :code:`docs/build/html` directory.

.. note::
    If you haven't already installed the package with the :code:`[dev]` extra as demonstrated above, the necessary dependencies for building the documentation can be installed by calling

        .. code-block:: console

            (venv) $ pip install --editable .[docs]
