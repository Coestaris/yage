message(FATAL_ERROR "Darwin graphics currently is not supported")

set(SRC_FILES ${SRC_FILES}
        src/platform/graphics/darwin/*.cpp
)

add_definitions(-DYAGE_PLATFORM_DARWIN)
