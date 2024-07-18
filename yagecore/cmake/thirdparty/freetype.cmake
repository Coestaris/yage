add_subdirectory(${PROJECT_DIR}/thirdparty/freetype)

if (NOT YAGECORE_STATIC)
    set(BUILD_SHARED_LIBS ON)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/thirdparty/freetype/include)
target_link_libraries(${PROJECT_NAME} PRIVATE freetype)

if (NOT YAGECORE_STATIC)
    target_compile_options(freetype PRIVATE -fPIC)
endif()
