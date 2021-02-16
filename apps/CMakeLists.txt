add_executable(${PROJECT_NAME}_app ${CMAKE_CURRENT_SOURCE_DIR}/app.cpp)
target_link_libraries(${PROJECT_NAME}_app PRIVATE JKQ::${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}_app
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_app ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_app
                   COMMENT "Creating symlink for ${PROJECT_NAME}_app"
                   VERBATIM)

add_executable(${PROJECT_NAME}_sim_app ${CMAKE_CURRENT_SOURCE_DIR}/sim_app.cpp)
target_link_libraries(${PROJECT_NAME}_sim_app PRIVATE JKQ::${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}_sim_app
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_sim_app ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_sim_app
                   COMMENT "Creating symlink for ${PROJECT_NAME}_sim_app"
                   VERBATIM)
