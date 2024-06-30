set(GLM_ENABLE_CXX_98 ON)
set(GLM_ENABLE_CXX_11 ON)
set(GLM_ENABLE_CXX_14 ON)
set(GLM_ENABLE_CXX_17 ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)

target_include_directories(yage PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
target_link_libraries(yage PRIVATE glm)
