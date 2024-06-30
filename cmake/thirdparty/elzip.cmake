# Apply git patches for windows compatibility
if (WIN32)
    execute_process(
            COMMAND git apply -p0 -v --summary --directory=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/elzip ${CMAKE_CURRENT_LIST_DIR}/elzip.patch
            COMMAND_ECHO STDOUT
            COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
            COMMAND git apply -p0 -v --summary --directory=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/elzip/extlibs/minizip ${CMAKE_CURRENT_LIST_DIR}/minizip.patch
            COMMAND_ECHO STDOUT
            COMMAND_ERROR_IS_FATAL ANY)
endif ()

set(SKIP_INSTALL_ALL ON)
set(ELZIP_BUILD_STATIC ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/elzip)
target_link_libraries(yage PRIVATE elzip)
