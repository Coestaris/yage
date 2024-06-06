# Include X11
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_libraries(${X11_LIBRARIES})

# Include GL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})

set(SRC_FILES ${SRC_FILES}
        src/platform/x11/*.cpp
)