message(FATAL_ERROR "Darwin graphics currently is not supported")

set(SRC_FILES ${SRC_FILES}
        src/darwin/*.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DYAGE_PLATFORM_DARWIN)
