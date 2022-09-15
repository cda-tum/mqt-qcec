# Contributing

Thank you for your interest in contributing to this project. Any contributions are greatly appreciated and we are happy to help you get started.

We use GitHub to [host code](https://github.com/cda-tum/qcec), to [track issues and feature requests](https://github.com/cda-tum/qcec/issues), as well as accept [pull requests](https://github.com/cda-tum/qcec/pulls).
See https://docs.github.com/en/get-started/quickstart for a general introduction to working with GitHub and contributing to projects.

## Types of Contributions

You can contribute in several ways:

### Report Bugs

Report bugs at https://github.com/cda-tum/qcec/issues using the _"🐛 Bug report"_ issue template. Please make sure to fill out all relevant information in the respective issue form.

### Fix Bugs

Look through the [GitHub Issues](https://github.com/cda-tum/qcec/issues) for bugs. Anything tagged with "bug" is open to whoever wants to try and fix it.

### Propose New Features

Propose new features at https://github.com/cda-tum/qcec/issues using the _"✨ Feature request"_ issue template. Please make sure to fill out all relevant information in the respective issue form.

### Implement New Features

Look through the [GitHub Issues](https://github.com/cda-tum/qcec/issues) for features. Anything tagged with "enhancement" is open to whoever wants to implement it. We highly appreciate external contributions to the project.

### Write Documentation

QCEC could always use some more [documentation](https://qcec.readthedocs.io/en/latest/), and we appreciate any help with that.

## Get Started

Ready to contribute? Check out the [Development guide](https://qcec.readthedocs.io/en/latest/install/Developers.html) to set up QCEC for local development and learn about the style guidelines and conventions used throughout the project.

## Contribution Guidelines

We value contributions from people with all levels of experience.
In particular if this is your first PR not everything has to be perfect.
We will guide you through the PR process.
Nevertheless, please try to follow the guidelines below as well as you can to help make the PR process quick and smooth.

### Core Guidelines

- ["Commit early and push often"](https://www.worklytics.co/blog/commit-early-push-often).
- Write meaningful commit messages (preferably using [gitmoji](https://gitmoji.dev) to give additional context to your commits).
- Focus on a single feature/bug at a time and only touch relevant files. Split multiple features into multiple contributions.
- If you added a new feature, you should add tests that ensure it works as intended. Furthermore, the new feature should be documented appropriately.
- If you fixed a bug, you should add tests that demonstrate that the bug has been fixed.
- Document your code thoroughly and write readable code.
- Keep your code clean. Remove any debug statements, left-over comments, or code unrelated to your contribution.

### Pull Request Workflow

- Create PRs early. It is ok to create work-in-progress PRs. You may mark these as draft PRs on GitHub.
- Describe your PR. Start with a descriptive title, reference any related issues by including the issue number in the PR description, and add a comprehensive description of the changes. We provide a PR template that you can (and should) follow to create a PR.
- Whenever a PR is created or updated, several workflows on all supported platforms and versions of Python are executed. Make sure your PR passes _all_ these continuous integration (CI) checks. Here are some tips for finding the cause of certain failures:
  - If any of the _CI/\*_ checks fail, this most likely indicates build errors or test failures in the C++ part of the code base. Look through the respective logs on GitHub for any error or failure messages.
  - If any of the _Deploy to PyPI/\*_ checks fail, this indicates an error in the Python bindings or creation of the Python wheels and/or source distribution. Look through the respective logs on GitHub for any error or failure messages.
  - If any of the _Python CI/\*_ checks fail, this indicates an error in the Python part of the code base. Look through the respective logs on GitHub for any error or failure messages.
  - If any of the _codecov/\*_ checks fail, this means that your changes are not appropriately covered by tests or that the overall project coverage decreased too much. Ensure that you include tests for all your changes in the PR.
  - If the _docs/readthedocs.org:qcec_ check fails, the documentation could not be built properly. Inspect the corresponding log file for any errors.
  - If _cpp-linter_ comments on your PR with a list of warnings, these have been raised by `clang-tidy` when checking the C++ part of your changes for warnings or style guideline violations. The individual messages frequently provide helpful suggestions on how to fix the warnings.
  - If the _pre-commit.ci_ check fails, some of the `pre-commit` checks failed and could not be fixed automatically by the _pre-commit.ci_ bot. Such failures are most likely related to the Python part of the code base. The individual log messages frequently provide helpful suggestions on how to fix the warnings.
- Once your PR is ready, change it from a draft PR to a regular PR and request a review from one of the project maintainers.
- If your PR gets a "Changes requested" review, you will need to address the feedback and update your PR by pushing to the same branch. You don't need to close the PR and open a new one. Respond to review comments on the PR (e.g., with "done 👍"). Be sure to re-request review once you have made changes after a code review so that maintainers know that the requests have been addressed.

---

This document was inspired by and partially adapted from

- https://matplotlib.org/stable/devel/coding_guide.html
- https://opensource.creativecommons.org/contributing-code/pr-guidelines/
- https://yeoman.io/contributing/pull-request.html
- https://github.com/scikit-build/scikit-build
