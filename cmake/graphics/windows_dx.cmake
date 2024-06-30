message(FATAL_ERROR "Windows (DirectX) graphics currently is not supported")

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/win_dx/*.cpp
)

target_compile_definitions(yage PRIVATE -DYAGE_PLATFORM_WIN_DX)
