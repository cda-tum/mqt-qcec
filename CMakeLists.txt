# set required cmake version
cmake_minimum_required(VERSION 3.19...3.28)

project(
  qcec
  LANGUAGES CXX
  DESCRIPTION "MQT QCEC - A tool for Quantum Circuit Equivalence Checking")

option(BUILD_MQT_QCEC_TESTS "Also build tests for the MQT QCEC project" ON)
option(BUILD_MQT_QCEC_BINDINGS "Build the MQT QCEC Python bindings" OFF)

if(BUILD_MQT_QCEC_BINDINGS)
  # ensure that the BINDINGS option is set
  set(BINDINGS
      ON
      CACHE INTERNAL "Enable settings related to Python bindings")
  set(Python_FIND_VIRTUALENV
      FIRST
      CACHE STRING "Give precedence to virtualenvs when searching for Python")
  set(Python_ARTIFACTS_INTERACTIVE
      ON
      CACHE
        BOOL
        "Prevent multiple searches for Python and instead cache the results.")
  # top-level call to find Python
  find_package(
    Python 3.8 REQUIRED
    COMPONENTS Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.SABIModule)
endif()

include(cmake/ExternalDependencies.cmake)

# add main library code
add_subdirectory(src)

# add test code
if(BUILD_MQT_QCEC_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()
