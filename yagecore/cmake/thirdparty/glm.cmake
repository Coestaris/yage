set(REPO_PATH ${THIRDPARTY_DIR}/glm)
set(REPO_URL "https://github.com/g-truc/glm.git")
set(REPO_COMMIT "33b4a621a697a305bc3a7610d290677b96beb181")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "glm not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${THIRDPARTY_DIR})
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
endif()

if (SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()

set(GLM_ENABLE_CXX_98 ON)
set(GLM_ENABLE_CXX_11 ON)
set(GLM_ENABLE_CXX_14 ON)
set(GLM_ENABLE_CXX_17 ON)
set(MESSAGE_QUIET ON)
add_subdirectory(${REPO_PATH})
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRDPARTY_DIR}/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

target_compile_options(glm PRIVATE ${YAGE_CXX_COMPILE_FLAGS})
target_link_options(glm PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(glm PRIVATE -fPIC)
endif()
