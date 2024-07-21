set(REPO_PATH ${PROJECT_DIR}/thirdparty/FreeImage)
set(REPO_URL "https://github.com/danoli3/FreeImage.git")
set(REPO_COMMIT "f3dd8949e5ecf03fa625731a1adc7de5fa7bc2cc")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "FreeImage not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH}/../)
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
endif()

# TODO: Find a way to build FreeImage as DLL on Windows
#if (NOT SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
#endif()

set(MESSAGE_QUIET ON)
add_subdirectory(${PROJECT_DIR}/thirdparty/FreeImage)
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PRIVATE ${REPO_PATH}/Source)
target_link_libraries(${PROJECT_NAME} PRIVATE FreeImage::FreeImage)
target_compile_definitions(${PROJECT_NAME} PRIVATE FREEIMAGE_LIB)

target_compile_options(FreeImage PRIVATE ${YAGE_CXX_COMPILE_FLAGS})
target_link_options(FreeImage PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(FreeImage PRIVATE -fPIC)
endif()