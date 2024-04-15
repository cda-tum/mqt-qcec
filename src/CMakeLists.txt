add_library(
  ${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/include/checker
  ${PROJECT_SOURCE_DIR}/include/Configuration.hpp
  ${PROJECT_SOURCE_DIR}/include/EquivalenceCriterion.hpp
  ${PROJECT_SOURCE_DIR}/include/EquivalenceCheckingManager.hpp
  ${PROJECT_SOURCE_DIR}/include/ThreadSafeQueue.hpp
  Configuration.cpp
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
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include
                                                  ${PROJECT_BINARY_DIR}/include)

# link to the MQT::Core libraries
target_link_libraries(${PROJECT_NAME} PUBLIC MQT::CoreDD MQT::CoreZX)
target_link_libraries(${PROJECT_NAME} PRIVATE MQT::ProjectWarnings
                                              MQT::ProjectOptions)

# add MQT alias
add_library(MQT::QCEC ALIAS ${PROJECT_NAME})

if(BUILD_MQT_QCEC_BINDINGS)
  add_subdirectory(python)
endif()
