message(FATAL_ERROR "Wayland (OpenGL) graphics currently is not supported")

set(SRC_FILES ${SRC_FILES}
        src/wayland/*.cpp
        src/gl/*.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DYAGE_PLATFORM_WAYLAND)
