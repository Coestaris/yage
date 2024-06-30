# Include X11
find_package(X11 REQUIRED)
target_include_directories(yage PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(yage PRIVATE ${X11_LIBRARIES})

# Include GL
find_package(OpenGL REQUIRED)
target_include_directories(yage PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(yage PRIVATE ${OPENGL_LIBRARIES})

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/x11/*.cpp
        src/platform/graphics/gl/*.cpp
)

target_compile_definitions(yage PRIVATE -DYAGE_PLATFORM_X11)