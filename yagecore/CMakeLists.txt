cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME yagecore)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

option(YAGECORE_BACKEND_X11_GL OFF)
option(YAGECORE_BACKEND_WAYLAND_GL OFF)
option(YAGECORE_BACKEND_DARWIN OFF)
option(YAGECORE_BACKEND_WIN_GL OFF)
option(YAGECORE_BACKEND_WIN_DX OFF)
option(YAGECORE_STATIC "Build yagepack as a static library" OFF)

# For now, build static for Windows
if (WIN32)
    set(YAGECORE_STATIC ON)
endif()

if (YAGECORE_STATIC)
    add_library(${PROJECT_NAME} STATIC
            ${sources}
    )
else()
    add_library(${PROJECT_NAME} SHARED
            ${sources}
    )
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
endif()

if (NOT BACKEND_X11_GL AND NOT BACKEND_WAYLAND_GL AND NOT BACKEND_DARWIN AND NOT BACKEND_WIN_GL AND NOT BACKEND_WIN_DX)
    message(STATUS "No platform specified, using auto-detection")
    if (UNIX)
        # Check if X11 is available
        if (NOT DEFINED ENV{DISPLAY})
            # Check if Wayland is available
            if (EXISTS /usr/share/wayland-sessions)
                message(STATUS "Detected Wayland (GL) platform")
                set(BACKEND_WAYLAND_GL 1)
            else ()
                message(STATUS "No display server found, defaulting to X11 (GL)")
                set(BACKEND_X11_GL 1)
            endif ()
        else ()
            message(STATUS "Detected X11 (GL) platform")
            set(BACKEND_X11_GL 1)
        endif ()
    elseif (WIN32)
        message(STATUS "Detected Windows (GL) platform")
        set(BACKEND_WIN_GL 1)
    elseif (APPLE)
        message(STATUS "Detected Darwin (GL) platform")
        set(BACKEND_DARWIN 1)
    else ()
        message(FATAL_ERROR "Unsupported platform")
    endif ()
endif ()

if (BACKEND_DARWIN AND NOT APPLE)
    message(FATAL_ERROR "Darwin backend is only available on macOS")
endif ()
if ((BACKEND_WIN_GL OR BACKEND_WIN_DX) AND NOT WIN32)
    message(FATAL_ERROR "Windows backend is only available on Windows")
endif ()

# Here, to not affect the build process by our options
include(cmake/thirdparty/FreeImage.cmake)
include(cmake/thirdparty/glm.cmake)
include(cmake/thirdparty/fastgltf.cmake)
include(cmake/thirdparty/freetype.cmake)
include(cmake/glfuncs/glfuncs.cmake)

# Setup compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(cxx_flags
        -Wall -Wextra -Werror -lpthread
        -Wno-unused-parameter -Wno-unused-variable
        -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field
        -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs -Werror=old-style-cast
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -DCOLLECT_DRAW_STATISTICS=1 -DUSE_DEBUG ${cxx_flags})
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -g0 -Os ${cxx_flags})
endif ()

# Setup common source files
set(SRC_FILES
    src/*.cpp
    src/graphics/*.cpp
)

# Add platform specific files
if (UNIX)
    include(cmake/linux.cmake)
elseif (WIN32)
    include(cmake/windows.cmake)
elseif (APPLE)
    include(cmake/darwin.cmake)
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

# Add backend specific files
if (BACKEND_X11_GL)
    include(cmake/graphics/x11_gl.cmake)
elseif (BACKEND_WAYLAND_GL)
    include(cmake/graphics/wayland_gl.cmake)
elseif (BACKEND_DARWIN)
    include(cmake/graphics/darwin.cmake)
elseif (BACKEND_WIN_GL)
    include(cmake/graphics/windows_gl.cmake)
elseif (BACKEND_WIN_DX)
    include(cmake/graphics/windows_dx.cmake)
else ()
    message(FATAL_ERROR "Unsupported backend")
endif ()

foreach (src_file ${SRC_FILES})
    file(GLOB src_files ${src_file})
    list(APPEND PLAIN_SRC_FILES ${src_files})
endforeach ()

target_include_directories(${PROJECT_NAME} PUBLIC
        include
)
target_sources(${PROJECT_NAME} PRIVATE ${PLAIN_SRC_FILES})

file(GLOB_RECURSE ___src *.cpp *.hpp)
foreach (file ${___src})
    get_filename_component(filename ${file} NAME)
    set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS "__SHORT_FILE__=\"${filename}\";__USE_SHORT_FILE__=1")
endforeach ()
