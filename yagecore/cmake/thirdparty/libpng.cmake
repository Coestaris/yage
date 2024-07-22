# To build static single binary, we need to build libpng as a static library
if (NOT SINGLEBIN AND NOT FORCE_BUILD_LIBRARIES)
    find_package(PNG)
    if (PNG_FOUND)
        message(STATUS "Libpng found. Using system library.")
        target_link_libraries(${PROJECT_NAME} PRIVATE PNG::PNG)
        return()
    endif ()
endif ()

set(REPO_PATH ${THIRDPARTY_DIR}/libpng)
set(REPO_URL "https://github.com/pnggroup/libpng")
set(REPO_COMMIT "e4a31f024b6158aaaf55a43502f574d5f5d1c894")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "Libpng not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${THIRDPARTY_DIR})
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY "${REPO_PATH}")
endif ()

if (SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
    set(PNG_SHARED OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
    set(PNG_SHARED ON)
endif ()
set(PNG_TESTS OFF)
set(PNG_TOOLS OFF)
set(PNG_EXECUTABLES OFF)

set(MESSAGE_QUIET ON)
add_subdirectory(${REPO_PATH})
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PRIVATE ${THIRDPARTY_DIR})

if (SINGLEBIN)
    set(libpng png_static)
else ()
    set(libpng png_shared)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${libpng})

target_compile_options(${libpng} PRIVATE ${YAGE_CXX_COMPILE_FLAGS})
target_link_options(${libpng} PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(${libpng} PRIVATE -fPIC)
endif ()