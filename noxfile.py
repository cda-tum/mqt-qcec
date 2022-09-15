from __future__ import annotations

import os

import nox
from nox.sessions import Session

nox.options.sessions = ["lint", "tests"]

PYTHON_ALL_VERSIONS = ["3.7", "3.8", "3.9", "3.10"]

if os.environ.get("CI", None):
    nox.options.error_on_missing_interpreters = True


@nox.session(python=PYTHON_ALL_VERSIONS)
def tests(session: Session) -> None:
    """Run the test suite."""
    session.install("-e", ".[test]")
    session.run("pytest", *session.posargs)


@nox.session(python=PYTHON_ALL_VERSIONS)
def coverage(session: Session) -> None:
    """Run the test suite and generate a coverage report."""
    session.install("-e", ".[coverage]")
    session.run("pytest", "--cov", *session.posargs)


@nox.session
def lint(session: Session) -> None:
    """Lint the Python part of the codebase."""
    session.install("pre-commit")
    session.run("pre-commit", "run", "--all-files", *session.posargs)  # --show-diff-on-failure


@nox.session
def pylint(session: Session) -> None:
    """Run pylint."""

    session.install("pylint")
    session.install("-e", ".")
    session.run("pylint", "mqt.qcec", "--extension-pkg-allow-list=mqt.qcec.pyqcec", *session.posargs)


@nox.session
def mypy(session: Session) -> None:
    """Run mypy."""

    session.install("pre-commit")
    session.run("pre-commit", "run", "--all-files", "--hook-stage", "manual", "mypy", *session.posargs)


@nox.session
def docs(session: Session) -> None:
    """Build the documentation."""
    session.install(".[docs]")
    session.chdir("docs")
    session.run("sphinx-build", "-M", "html", "source", "_build")

    if session.posargs:
        if "serve" in session.posargs:
            print("Launching docs at http://localhost:8000/ - use Ctrl-C to quit")
            session.run("python", "-m", "http.server", "8000", "-d", "_build/html")
        else:
            print("Unsupported argument to docs")
