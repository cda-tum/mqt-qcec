"""Nox sessions."""

from __future__ import annotations

import os

import nox

nox.options.sessions = ["lint", "pylint", "tests"]

PYTHON_ALL_VERSIONS = ["3.8", "3.9", "3.10", "3.11", "3.12"]

if os.environ.get("CI", None):
    nox.options.error_on_missing_interpreters = True


@nox.session(reuse_venv=True)
def lint(session: nox.Session) -> None:
    """Lint the Python part of the codebase using pre-commit.

    Simply execute `nox -rs lint` to run all configured hooks.
    """
    session.install("pre-commit")
    session.run("pre-commit", "run", "--all-files", *session.posargs)


@nox.session(reuse_venv=True)
def pylint(session: nox.Session) -> None:
    """Run PyLint.

    Simply execute `nox -rs pylint` to run PyLint.
    """
    session.install("scikit-build-core[pyproject]", "setuptools_scm")
    session.install("--no-build-isolation", "-ve.[dev]", "pylint")
    session.run("pylint", "mqt.qcec", *session.posargs)


@nox.session(reuse_venv=True, python=PYTHON_ALL_VERSIONS)
def tests(session: nox.Session) -> None:
    """Run the test suite.

    Simply execute `nox -rs tests` to run all tests.
    """
    posargs = list(session.posargs)
    env = {"PIP_DISABLE_PIP_VERSION_CHECK": "1"}
    install_arg = "-ve.[coverage]" if "--cov" in posargs else "-ve.[test]"
    if "--cov" in posargs:
        posargs.append("--cov-config=pyproject.toml")

    session.install("scikit-build-core[pyproject]", "setuptools_scm")
    session.install("--no-build-isolation", install_arg)
    session.run("pip", "show", "qiskit-terra")
    session.run("pytest", *posargs, env=env)


@nox.session(reuse_venv=True)
def min_qiskit_version(session: nox.Session) -> None:
    """Installs the minimum supported version of Qiskit, runs the test suite and collects the coverage."""
    session.install("qiskit-terra~=0.20.0")
    session.install("scikit-build-core[pyproject]", "setuptools_scm")
    session.install("--no-build-isolation", "-ve.[coverage]")
    session.run("pip", "show", "qiskit-terra")
    session.run("pytest", "--cov", *session.posargs)


@nox.session(reuse_venv=True)
def docs(session: nox.Session) -> None:
    """Build the documentation.

    Simply execute `nox -rs docs` to locally build and serve the docs.
    """
    session.install("sphinx-autobuild")
    session.install("scikit-build-core[pyproject]", "setuptools_scm")
    session.install("--no-build-isolation", "-ve.[docs]")

    session.run("sphinx-autobuild", "docs/source", "docs/_build/html", "--open-browser")
