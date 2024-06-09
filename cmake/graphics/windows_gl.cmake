# Include GL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/win_gl/*.cpp
        src/platform/graphics/gl/*.cpp
)

add_definitions(-DYAGE_PLATFORM_WIN_GL)