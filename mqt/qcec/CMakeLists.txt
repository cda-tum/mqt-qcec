pybind11_add_module(py${PROJECT_NAME} bindings.cpp)
target_link_libraries(py${PROJECT_NAME} PRIVATE ${PROJECT_NAME} MQT::CorePython)
target_compile_definitions(py${PROJECT_NAME}
                           PRIVATE VERSION_INFO=${QCEC_VERSION_INFO})
