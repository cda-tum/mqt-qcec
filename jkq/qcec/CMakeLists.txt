pybind11_add_module(py${PROJECT_NAME} bindings.cpp)
target_link_libraries(py${PROJECT_NAME} PUBLIC ${PROJECT_NAME})
set_target_properties(py${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
