# Include X11
find_package(X11 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})

# Include GL
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES})

set(SRC_FILES ${SRC_FILES}
        src/x11/*.cpp
        src/gl/*.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DYAGE_PLATFORM_X11)