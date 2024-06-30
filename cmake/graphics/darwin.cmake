message(FATAL_ERROR "Darwin graphics currently is not supported")

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/darwin/*.cpp
)

target_compile_definitions(yage PRIVATE -DYAGE_PLATFORM_DARWIN)
