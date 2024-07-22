set(REPO_PATH ${THIRDPARTY_DIR}/jsoncpp)
set(REPO_URL "https://github.com/open-source-parsers/jsoncpp.git")
set(REPO_COMMIT "69098a18b9af0c47549d9a271c054d13ca92b006")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "jsoncpp not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${THIRDPARTY_DIR})
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
endif ()


set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(JSONCPP_WITH_TESTS OFF)

if (SINGLEBIN)
    set(BUILD_OBJECT_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_OBJECT_LIBS OFF)
    set(BUILD_STATIC_LIBS OFF)
    set(BUILD_SHARED_LIBS ON)
endif ()

set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)

set(MESSAGE_QUIET ON)
add_subdirectory(${REPO_PATH})
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${REPO_PATH}/include)
if (SINGLEBIN)
    set(jsoncpp jsoncpp_static)
else ()
    set(jsoncpp jsoncpp_lib)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${jsoncpp})
target_compile_options(${jsoncpp} PRIVATE ${YAGE_CXX_COMPILE_FLAGS})

target_link_options(${jsoncpp} PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(${jsoncpp} PRIVATE -fPIC)
endif ()