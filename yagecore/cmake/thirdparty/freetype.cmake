# To build static single binary, we need to build FreeType as a static library
if (NOT SINGLEBIN)
    find_package(Freetype)
    if (FREETYPE_FOUND)
        message(STATUS "Freetype found. Using system library.")
        target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype)
        return()
    endif()
endif()

message(STATUS "Freetype not found. Building from source.")

set(REPO_PATH ${PROJECT_DIR}/thirdparty/freetype)
set(REPO_URL "https://github.com/freetype/freetype.git")
set(REPO_COMMIT "37cefe33b284d0bad4ec52bcccc1a8c2d8704340")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "freetype not found. Clone it.")
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
add_subdirectory(${PROJECT_DIR}/thirdparty/freetype)
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PRIVATE ${REPO_PATH}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE freetype)

target_compile_options(freetype PRIVATE ${YAGE_CXX_COMPILE_FLAGS})
target_link_options(freetype PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(freetype PRIVATE -fPIC)
endif()