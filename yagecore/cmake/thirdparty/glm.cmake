# Try to find GLM
if (NOT SINGLEBIN AND NOT FORCE_BUILD_LIBRARIES)
    find_package(glm QUIET)
    if (glm_FOUND)
        message(STATUS "GLM found. Using system library.")
        target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
        return()
    endif ()
endif ()

set(REPO_PATH ${THIRDPARTY_DIR}/glm)
set(REPO_URL "https://github.com/g-truc/glm.git")
set(REPO_COMMIT "33b4a621a697a305bc3a7610d290677b96beb181")

message(STATUS "GLM not found. Building from source.")

if (NOT EXISTS ${REPO_PATH})
    message(STATUS "glm not found. Clone it.")
    execute_process(COMMAND git clone ${REPO_URL} ${REPO_PATH} --depth 1
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${THIRDPARTY_DIR})
    execute_process(COMMAND git checkout --force ${REPO_COMMIT}
            COMMAND_ERROR_IS_FATAL ANY
            WORKING_DIRECTORY ${REPO_PATH})
endif ()

if (SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()

if (USE_SSE4_2)
    set(GLM_ENABLE_SIMD_SSE4_2 ON CACHE BOOL "Enable SSE4.2 instructions")
elseif (USE_SSE4_1)
    set(GLM_ENABLE_SIMD_SSE4_1 ON CACHE BOOL "Enable SSE4.1 instructions")
elseif (USE_SSSE3)
    set(GLM_ENABLE_SIMD_SSSE3 ON CACHE BOOL "Enable SSSE3 instructions")
elseif (USE_SSE3)
    set(GLM_ENABLE_SIMD_SSE3 ON CACHE BOOL "Enable SSE3 instructions")
elseif (USE_SSE2)
    set(GLM_ENABLE_SIMD_SSE2 ON CACHE BOOL "Enable SSE2 instructions")
elseif (USE_NEON)
    set(GLM_TEST_ENABLE_SIMD_NEON ON CACHE BOOL "Enable ARM NEON instructions")
elseif (USE_AVX)
    set(GLM_ENABLE_SIMD_AVX ON CACHE BOOL "Enable AVX instructions")
elseif (USE_AVX2)
    set(GLM_ENABLE_SIMD_AVX2 ON CACHE BOOL "Enable AVX2 instructions")
endif ()

set(GLM_ENABLE_CXX_98 ON CACHE BOOL "Enable C++98")
set(GLM_ENABLE_CXX_11 ON CACHE BOOL "Enable C++11")
set(GLM_ENABLE_CXX_14 ON CACHE BOOL "Enable C++14")
set(GLM_ENABLE_CXX_17 ON CACHE BOOL "Enable C++17")
set(MESSAGE_QUIET ON)
add_subdirectory(${REPO_PATH})
set(MESSAGE_QUIET OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${THIRDPARTY_DIR}/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

target_compile_options(glm PRIVATE ${YAGE_CXX_COMPILE_FLAGS} ${Vc_ARCHITECTURE_FLAGS})
target_link_options(glm PRIVATE ${YAGE_CXX_LINK_FLAGS})
if (NOT SINGLEBIN)
    target_compile_options(glm PRIVATE -fPIC)
endif ()
