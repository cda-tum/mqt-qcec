pybind11_add_module(py${PROJECT_NAME} bindings.cpp)
target_link_libraries(py${PROJECT_NAME} PRIVATE ${PROJECT_NAME})

# Install directive for scikit-build-core
install(
  TARGETS py${PROJECT_NAME}
  DESTINATION .
  COMPONENT MQTQCECPythonModule)
