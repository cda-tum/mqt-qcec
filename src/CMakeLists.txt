set(MQT_QCEC_TARGET_NAME mqt-qcec)

if(MQT_QCEC_INSTALL)
  include(GNUInstallDirs)
  set(MQT_QCEC_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/mqt-qcec")
  set(MQT_QCEC_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/mqt-qcec")
  set(MQT_QCEC_TARGETS_EXPORT_NAME "mqt-qcec-targets")
  set(MQT_QCEC_CMAKE_CONFIG_TEMPLATE
      "${PROJECT_SOURCE_DIR}/cmake/mqt-qcec-config.cmake.in")
  set(MQT_QCEC_CMAKE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  set(MQT_QCEC_CMAKE_VERSION_CONFIG_FILE
      "${MQT_QCEC_CMAKE_CONFIG_DIR}/mqt-qcec-config-version.cmake")
  set(MQT_QCEC_CMAKE_PROJECT_CONFIG_FILE
      "${MQT_QCEC_CMAKE_CONFIG_DIR}/mqt-qcec-config.cmake")
  set(MQT_QCEC_CMAKE_PROJECT_TARGETS_FILE
      "${MQT_QCEC_CMAKE_CONFIG_DIR}/mqt-qcec-targets.cmake")

  if(APPLE)
    set(BASEPOINT @loader_path)
  else()
    set(BASEPOINT $ORIGIN)
  endif()
  list(APPEND CMAKE_INSTALL_RPATH ${BASEPOINT}
       ${BASEPOINT}/${CMAKE_INSTALL_LIBDIR})
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  list(APPEND CMAKE_INSTALL_RPATH ${BASEPOINT}/../core/${CMAKE_INSTALL_LIBDIR}
       ${BASEPOINT}/../../core/${CMAKE_INSTALL_LIBDIR})

endif()

if(NOT TARGET ${MQT_QCEC_TARGET_NAME})
  add_library(
    ${MQT_QCEC_TARGET_NAME}
    ${MQT_QCEC_INCLUDE_BUILD_DIR}/checker
    ${MQT_QCEC_INCLUDE_BUILD_DIR}/Configuration.hpp
    ${MQT_QCEC_INCLUDE_BUILD_DIR}/EquivalenceCriterion.hpp
    ${MQT_QCEC_INCLUDE_BUILD_DIR}/EquivalenceCheckingManager.hpp
    ${MQT_QCEC_INCLUDE_BUILD_DIR}/ThreadSafeQueue.hpp
    EquivalenceCheckingManager.cpp
    checker/EquivalenceChecker.cpp
    checker/dd/DDEquivalenceChecker.cpp
    checker/dd/DDConstructionChecker.cpp
    checker/dd/DDSimulationChecker.cpp
    checker/dd/DDAlternatingChecker.cpp
    checker/dd/applicationscheme/GateCostApplicationScheme.cpp
    checker/dd/simulation/StateGenerator.cpp
    checker/zx/ZXChecker.cpp)

  # set include directories
  target_include_directories(
    ${MQT_QCEC_TARGET_NAME}
    PUBLIC $<BUILD_INTERFACE:${MQT_QCEC_INCLUDE_BUILD_DIR}>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

  # link to the MQT::Core libraries
  target_link_libraries(${MQT_QCEC_TARGET_NAME} PUBLIC MQT::CoreDD MQT::CoreZX)
  target_link_libraries(${MQT_QCEC_TARGET_NAME} PRIVATE MQT::ProjectWarnings
                                                        MQT::ProjectOptions)

  # add MQT alias
  add_library(MQT::QCEC ALIAS ${MQT_QCEC_TARGET_NAME})
  set_target_properties(
    ${MQT_QCEC_TARGET_NAME}
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               EXPORT_NAME QCEC)

  include(GenerateExportHeader)
  generate_export_header(${MQT_QCEC_TARGET_NAME} BASE_NAME mqt_qcec)
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${MQT_QCEC_TARGET_NAME}
                               PUBLIC MQT_QCEC_STATIC_DEFINE)
  endif()
endif()

if(BUILD_MQT_QCEC_BINDINGS)
  add_subdirectory(python)
endif()

if(MQT_QCEC_INSTALL)
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    ${MQT_QCEC_CMAKE_CONFIG_TEMPLATE} ${MQT_QCEC_CMAKE_PROJECT_CONFIG_FILE}
    INSTALL_DESTINATION ${MQT_QCEC_CONFIG_INSTALL_DIR}
    NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(
    ${MQT_QCEC_CMAKE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

  install(FILES ${MQT_QCEC_CMAKE_PROJECT_CONFIG_FILE}
                ${MQT_QCEC_CMAKE_VERSION_CONFIG_FILE}
          DESTINATION ${MQT_QCEC_CONFIG_INSTALL_DIR})

  install(
    TARGETS ${MQT_QCEC_TARGET_NAME}
    EXPORT ${MQT_QCEC_TARGETS_EXPORT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT ${MQT_QCEC_TARGET_NAME}_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${MQT_QCEC_TARGET_NAME}_Runtime
            NAMELINK_COMPONENT ${MQT_QCEC_TARGET_NAME}_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${MQT_QCEC_TARGET_NAME}_Development
    INCLUDES
    DESTINATION ${MQT_QCEC_INCLUDE_INSTALL_DIR})

  install(DIRECTORY ${MQT_QCEC_INCLUDE_BUILD_DIR}/
          DESTINATION ${MQT_QCEC_INCLUDE_INSTALL_DIR})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mqt_qcec_export.h
          DESTINATION ${MQT_QCEC_INCLUDE_INSTALL_DIR})

  install(
    EXPORT ${MQT_QCEC_TARGETS_EXPORT_NAME}
    FILE ${MQT_QCEC_PROJECT_TARGETS_FILE}
    NAMESPACE MQT::
    DESTINATION ${MQT_QCEC_CONFIG_INSTALL_DIR}
    COMPONENT ${MQT_QCEC_TARGET_NAME}_Development)
endif()
