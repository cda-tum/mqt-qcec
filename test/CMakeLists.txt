# collect all test files
file(GLOB_RECURSE TEST_FILES "*.cpp")

# add test executable
package_add_test(mqt-qcec-test MQT::QCEC ${TEST_FILES})

# link to MQT::CoreAlgorithms library for dynamic circuit tests
target_link_libraries(mqt-qcec-test PRIVATE MQT::CoreAlgorithms MQT::CoreQASM)
