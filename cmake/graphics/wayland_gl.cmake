message(FATAL_ERROR "Wayland (OpenGL) graphics currently is not supported")

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/wayland/*.cpp
        src/platform/graphics/gl/*.cpp
)

target_compile_definitions(yage PRIVATE -DYAGE_PLATFORM_WAYLAND)
