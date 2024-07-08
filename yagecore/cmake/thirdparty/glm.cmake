set(GLM_ENABLE_CXX_98 ON)
set(GLM_ENABLE_CXX_11 ON)
set(GLM_ENABLE_CXX_14 ON)
set(GLM_ENABLE_CXX_17 ON)
add_subdirectory(${PROJECT_DIR}/thirdparty/glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_DIR}/thirdparty/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
