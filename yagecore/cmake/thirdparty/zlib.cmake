set(ZLIB_PATH ${THIRDPARTY_DIR}/zlib)
set(ZLIB_URL "https://github.com/madler/zlib")
set(ZLIB_COMMIT "ceadaf28dfa48dbf238a0ddb884d4c543b4170e8")

find_package(ZLIB)
if (ZLIB_FOUND AND NOT ("${ZLIB_INCLUDE_DIRS}" STREQUAL "${ZLIB_PATH}"))
    message(STATUS "ZLIB found at ${ZLIB_INCLUDE_DIRS}. Using system library.")
    return()
endif ()

if (NOT EXISTS ${ZLIB_PATH})
    execute_process(COMMAND git clone ${ZLIB_URL} ${ZLIB_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${THIRDPARTY_DIR})
    execute_process(COMMAND git checkout --force ${ZLIB_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY "${ZLIB_PATH}")
endif ()

add_subdirectory(${ZLIB_PATH})

if (NOT SINGLEBIN)
    # Force copy zlib to the build directory
    target_link_libraries(${PROJECT_NAME} PRIVATE zlib)
    add_library(ZLIB::ZLIB ALIAS zlib)
else ()
    set(ZLIB_LIBRARIES zlibstatic CACHE FILEPATH "ZLIB library")
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif ()


# Tell libpng where to find ZLIB
# set(ZLIB_ROOT ${ZLIB_PATH} CACHE PATH "ZLIB root directory")
set(FAKE_ZLIB_FOUND ON CACHE BOOL "ZLIB found")
set(ZLIB_FOUND ON CACHE BOOL "ZLIB found")
set(ZLIB_INCLUDE_DIRS ${ZLIB_PATH} CACHE PATH "ZLIB include directory")
set(ZLIB_LIBRARIES zlib CACHE FILEPATH "ZLIB library")
