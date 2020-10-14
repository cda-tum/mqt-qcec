add_subdirectory("../../extern/pybind11" "extern/pybind11")
add_library(pybind11 ALIAS pybind11::pybind11)
add_subdirectory("../../extern/pybind11_json" "extern/pybind11_json")

pybind11_add_module(_${PROJECT_NAME} qcecPythonBindings.cpp EXCLUDE_FROM_ALL)
target_link_libraries(_${PROJECT_NAME} PUBLIC pybind11_json ${PROJECT_NAME})
