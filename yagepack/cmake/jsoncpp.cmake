set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(JSONCPP_WITH_TESTS OFF)

if (NOT YAGEPACK_STATIC)
    set(BUILD_OBJECT_LIBS ON)
    set(BUILD_STATIC_LIBS OFF)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_OBJECT_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
endif()

set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)

add_subdirectory(${PROJECT_DIR}/thirdparty/jsoncpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_DIR}/thirdparty/jsoncpp/include)
if (NOT YAGEPACK_STATIC)
    target_link_libraries(${PROJECT_NAME} PUBLIC jsoncpp_lib)
    target_compile_options(jsoncpp_lib PRIVATE -fPIC)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(jsoncpp_lib PRIVATE -g3 -O0)
        target_link_options(jsoncpp_lib PRIVATE -g3 -O0)
    else ()
        target_compile_options(jsoncpp_lib PRIVATE -g0 -O2 -s)
        target_link_options(jsoncpp_lib PRIVATE -g0 -O2 -s)
    endif ()

else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC jsoncpp_static)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(jsoncpp_static PRIVATE -g3 -O0)
        target_link_options(jsoncpp_static PRIVATE -g3 -O0)
    else ()
        target_compile_options(jsoncpp_static PRIVATE -g0 -O2 -s)
        target_link_options(jsoncpp_static PRIVATE -g0 -O2 -s)
    endif ()

endif()
