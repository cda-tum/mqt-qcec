pybind11_add_module(pyqcec bindings.cpp)
target_link_libraries(pyqcec PRIVATE pybind11::thin_lto MQT::QCEC
                                     MQT::ProjectOptions MQT::ProjectWarnings)

# Install directive for scikit-build-core
install(
  TARGETS pyqcec
  DESTINATION .
  COMPONENT MQTQCECPythonModule)
