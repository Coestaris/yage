if (NOT YAGEPACK_STATIC)
    set(BUILD_SHARED_LIBS ON)
endif()

set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(JSONCPP_WITH_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_OBJECT_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)

add_subdirectory(${PROJECT_DIR}/thirdparty/jsoncpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_DIR}/thirdparty/jsoncpp/include)
target_link_libraries(${PROJECT_NAME} PUBLIC jsoncpp_static)
if (NOT YAGEPACK_STATIC)
    target_compile_options(jsoncpp_static PRIVATE -fPIC)
endif()