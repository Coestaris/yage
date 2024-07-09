add_subdirectory(${PROJECT_DIR}/thirdparty/fastgltf)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/thirdparty/fastgltf/include)
target_link_libraries(${PROJECT_NAME} PRIVATE fastgltf)
