# Include GL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})

set(SRC_FILES ${SRC_FILES}
        src/platform/gl/*.cpp
)
