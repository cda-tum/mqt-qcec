# set required cmake version
cmake_minimum_required(VERSION 3.19...3.28)

project(
  mqt-qcec
  LANGUAGES CXX
  DESCRIPTION "MQT QCEC - A tool for Quantum Circuit Equivalence Checking")

include(cmake/Python.cmake)
option(BUILD_MQT_QCEC_BINDINGS "Build the MQT QCEC Python bindings" OFF)
if(BUILD_MQT_QCEC_BINDINGS)
  # ensure that the BINDINGS option is set
  set(BINDINGS
      ON
      CACHE INTERNAL "Enable settings related to Python bindings")
  find_python()
endif()

# check if this is the master project or used via add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(MQT_QCEC_MASTER_PROJECT ON)
else()
  set(MQT_QCEC_MASTER_PROJECT OFF)
endif()

option(MQT_QCEC_INSTALL "Generate installation instructions for MQT QCEC"
       ${MQT_QCEC_MASTER_PROJECT})
option(BUILD_MQT_QCEC_TESTS "Also build tests for the MQT QCEC project"
       ${MQT_QCEC_MASTER_PROJECT})

# try to determine the project version
include(cmake/GetVersion.cmake)
get_mqt_qcec_version()

project(
  mqt-qcec
  LANGUAGES CXX
  VERSION ${MQT_QCEC_VERSION}
  DESCRIPTION "MQT QCEC - A tool for Quantum Circuit Equivalence Checking")

include(cmake/ExternalDependencies.cmake)

# set the include directory for the build tree
set(MQT_QCEC_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/mqt-qcec")

# add main library code
add_subdirectory(src)

# add test code
if(BUILD_MQT_QCEC_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()

if(MQT_QCEC_MASTER_PROJECT)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)
  add_custom_target(
    uninstall COMMAND ${CMAKE_COMMAND} -P
                      ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
else()
  set(mqt-qcec_FOUND
      TRUE
      CACHE INTERNAL "True if mqt-qcec is found on the system")
endif()
