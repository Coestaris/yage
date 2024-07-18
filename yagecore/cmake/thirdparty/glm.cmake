if (NOT YAGECORE_STATIC)
    set(BUILD_SHARED_LIBS ON)
endif()

set(GLM_ENABLE_CXX_98 ON)
set(GLM_ENABLE_CXX_11 ON)
set(GLM_ENABLE_CXX_14 ON)
set(GLM_ENABLE_CXX_17 ON)
add_subdirectory(${PROJECT_DIR}/thirdparty/glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_DIR}/thirdparty/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(glm PRIVATE -g3 -O0)
    target_link_options(glm PRIVATE -g3 -O0)
else ()
    target_compile_options(glm PRIVATE -g0 -O2 -s)
    target_link_options(glm PRIVATE -g0 -02 -s)
endif ()

