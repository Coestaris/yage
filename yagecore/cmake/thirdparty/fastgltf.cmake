set(REPO_PATH ${PROJECT_DIR}/thirdparty/fastgltf)
set(REPO_URL "https://github.com/spnda/fastgltf.git")
set(REPO_COMMIT "4d5c3688059cd2b10d01d6e90ee7196e76b089f1")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "fastgltf not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH}/../)
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
endif()

if (SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()

set(MESSAGE_QUIET ON)
add_subdirectory(${REPO_PATH})
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PRIVATE ${REPO_PATH}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE fastgltf)

target_compile_options(fastgltf PRIVATE ${YAGE_CXX_COMPILE_FLAGS})
target_link_options(fastgltf PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(fastgltf PRIVATE -fPIC)
endif()