# 	path = yagepack/thirdparty/elzip
#	url = https://github.com/Sygmei/11Zip
#	commit = 516e161d5c96aa8f2603fb30b10b7770a87332c2
set(REPO_PATH ${PROJECT_DIR}/thirdparty/elzip)
set(REPO_URL "https://github.com/Sygmei/11Zip.git")
set(REPO_COMMIT "516e161d5c96aa8f2603fb30b10b7770a87332c2")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "elzip not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH}/../)
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
    execute_process(COMMAND git submodule update --init --recursive
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
endif()

# Apply git patches for windows compatibility
if (WIN32)
    execute_process(
            COMMAND git apply -p0 -v --summary --directory=${PROJECT_DIR}/thirdparty/elzip ${CMAKE_CURRENT_LIST_DIR}/elzip.patch
            COMMAND_ECHO STDOUT
            COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
            COMMAND git apply -p0 -v --summary --directory=${PROJECT_DIR}/thirdparty/elzip/extlibs/minizip ${CMAKE_CURRENT_LIST_DIR}/minizip.patch
            COMMAND_ECHO STDOUT
            COMMAND_ERROR_IS_FATAL ANY)
endif ()

if (SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
    set(ELZIP_BUILD_STATIC OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
    set(ELZIP_BUILD_STATIC ON)
endif ()
set(SKIP_INSTALL_ALL ON)

set(MESSAGE_QUIET ON)
add_subdirectory(${REPO_PATH})
set(MESSAGE_QUIET OFF)

target_link_libraries(${PROJECT_NAME} PRIVATE elzip)

target_compile_options(elzip PRIVATE ${YAGE_CXX_COMPILE_FLAGS})
target_link_options(elzip PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(elzip PRIVATE -fPIC)
endif()