# Include GL
find_package(OpenGL REQUIRED)
target_include_directories(yage PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(yage PRIVATE ${OPENGL_LIBRARIES})

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/win_gl/*.cpp
        src/platform/graphics/gl/*.cpp
)

target_compile_definitions(yage PRIVATE -DYAGE_PLATFORM_WIN_GL)