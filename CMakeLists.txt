# set required cmake version
cmake_minimum_required(VERSION 3.19)

project(
  qcec
  LANGUAGES CXX
  DESCRIPTION "MQT QCEC - A tool for Quantum Circuit Equivalence Checking")

# check whether the submodule ``modulename`` is correctly cloned in the ``/extern`` directory.
macro(CHECK_SUBMODULE_PRESENT modulename)
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${modulename}/CMakeLists.txt")
    message(
      FATAL_ERROR
        "${modulename} submodule not cloned properly. \
        Please run `git submodule update --init --recursive` \
        from the main project directory")
  endif()
endmacro()

check_submodule_present(qfr)

# add main library code
add_subdirectory(src)

# add test code
option(BUILD_QCEC_TESTS "Also build tests for QMAP project")
if(BUILD_QCEC_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()

# add Python binding code
option(BINDINGS "Configure for building Python bindings")
if(BINDINGS)
  add_subdirectory(mqt/qcec)
endif()
