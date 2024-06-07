cmake_minimum_required(VERSION 3.16)

project(yage LANGUAGES CXX C)

option(BACKEND_X11_GL 0)
option(BACKEND_WAYLAND_GL 0)
option(BACKEND_DARWIN 0)
option(BACKEND_WIN_GL 0)
option(BACKEND_WIN_DX 0)

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

# Setup compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -lpthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs")

# Setup common source files
set(SRC_FILES
    src/core/*.cpp
    src/*.cpp
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
    include(cmake/x11_gl.cmake)
elseif (BACKEND_WAYLAND_GL)
    include(cmake/wayland_gl.cmake)
elseif (BACKEND_DARWIN)
    include(cmake/darwin.cmake)
elseif (BACKEND_WIN_GL)
    include(cmake/windows_gl.cmake)
elseif (BACKEND_WIN_DX)
    include(cmake/windows_dx.cmake)
else ()
    message(FATAL_ERROR "Unsupported backend")
endif ()

include(cmake/utils.cmake)

foreach (src_file ${SRC_FILES})
    file(GLOB_RECURSE src_files ${src_file})
    list(APPEND PLAIN_SRC_FILES ${src_files})
endforeach ()

util_add_short_filename_property(${PLAIN_SRC_FILES})

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(yage ${PLAIN_SRC_FILES})
