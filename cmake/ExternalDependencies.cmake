# Declare all external dependencies and make sure that they are available.

include(FetchContent)
set(FETCH_PACKAGES "")

if(BUILD_MQT_QCEC_BINDINGS)
  # Manually detect the installed mqt-core package.
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m mqt.core --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE mqt-core_DIR
    ERROR_QUIET)

  # Add the detected directory to the CMake prefix path.
  if(mqt-core_DIR)
    list(APPEND CMAKE_PREFIX_PATH "${mqt-core_DIR}")
    message(STATUS "Found mqt-core package: ${mqt-core_DIR}")
  endif()

  if(NOT SKBUILD)
    # Manually detect the installed pybind11 package.
    execute_process(
      COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE pybind11_DIR)

    # Add the detected directory to the CMake prefix path.
    list(APPEND CMAKE_PREFIX_PATH "${pybind11_DIR}")
  endif()

  # add pybind11 library
  find_package(pybind11 2.13.6 CONFIG REQUIRED)
endif()

# cmake-format: off
set(MQT_CORE_VERSION 3.0.0
    CACHE STRING "MQT Core version")
set(MQT_CORE_REV "8fbea945dc64c69264f00c631f51c12b019d8916"
    CACHE STRING "MQT Core identifier (tag, branch or commit hash)")
set(MQT_CORE_REPO_OWNER "munich-quantum-toolkit"
	CACHE STRING "MQT Core repository owner (change when using a fork)")
# cmake-format: on
FetchContent_Declare(
  mqt-core
  GIT_REPOSITORY https://github.com/${MQT_CORE_REPO_OWNER}/core.git
  GIT_TAG ${MQT_CORE_REV}
  FIND_PACKAGE_ARGS ${MQT_CORE_VERSION})
list(APPEND FETCH_PACKAGES mqt-core)

if(BUILD_MQT_QCEC_TESTS)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  set(GTEST_VERSION
      1.16.0
      CACHE STRING "Google Test version")
  set(GTEST_URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz)
  FetchContent_Declare(googletest URL ${GTEST_URL} FIND_PACKAGE_ARGS ${GTEST_VERSION} NAMES GTest)
  list(APPEND FETCH_PACKAGES googletest)
endif()

if(BUILD_MQT_QCEC_BINDINGS)
  # add pybind11_json library
  FetchContent_Declare(
    pybind11_json
    GIT_REPOSITORY https://github.com/pybind/pybind11_json
    FIND_PACKAGE_ARGS)
  list(APPEND FETCH_PACKAGES pybind11_json)
endif()

# Make all declared dependencies available.
FetchContent_MakeAvailable(${FETCH_PACKAGES})
