add_subdirectory("${PROJECT_SOURCE_DIR}/extern/pybind11" "extern/pybind11")
add_library(pybind11 ALIAS pybind11::pybind11)

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/pybind11_json" "extern/pybind11_json")

pybind11_add_module(py${PROJECT_NAME} bindings.cpp)
target_link_libraries(py${PROJECT_NAME} PUBLIC pybind11_json ${PROJECT_NAME})
