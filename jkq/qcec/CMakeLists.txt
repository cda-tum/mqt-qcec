pybind11_add_module(py${PROJECT_NAME} bindings.cpp)
target_link_libraries(py${PROJECT_NAME} PUBLIC ${PROJECT_NAME})
set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE TRUE)
