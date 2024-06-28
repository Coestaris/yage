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

# Here, to not affect the build process by our options
set(SKIP_INSTALL_ALL ON)
add_subdirectory(thirdparty/11Zip)
set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(JSONCPP_WITH_TESTS OFF)
set(BUILD_OBJECT_LIBS OFF)
set(BUILD_STATIC_LIBS OFF)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
add_subdirectory(thirdparty/jsoncpp)
set(BUILD_SHARED_LIBS ON)
add_subdirectory(thirdparty/free_image)
set(GLM_ENABLE_CXX_98 ON)
set(GLM_ENABLE_CXX_11 ON)
set(GLM_ENABLE_CXX_14 ON)
set(GLM_ENABLE_CXX_17 ON)
add_subdirectory(thirdparty/glm)

# Setup compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs -Werror=old-style-cast")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -lpthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -DUSE_DEBUG")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0 -DUSE_DEBUG")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif ()

# Setup common source files
set(SRC_FILES
        src/*.cpp
        src/core/*.cpp
        src/core/util/*.cpp

        src/app/*.cpp
        src/app/io/*.cpp
        src/app/threads/*.cpp
        src/app/assets/*.cpp
        src/app/assets/pack/*.cpp

        src/graphics/*.cpp

        src/platform/*.cpp
        src/platform/graphics/*.cpp
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

include(cmake/utils.cmake)

foreach (src_file ${SRC_FILES})
    file(GLOB src_files ${src_file})
    list(APPEND PLAIN_SRC_FILES ${src_files})
endforeach ()

util_add_short_filename_property(${PLAIN_SRC_FILES})

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(yage ${PLAIN_SRC_FILES}
        src/graphics/graphics.hpp)

target_link_libraries(yage elzip)
target_link_libraries(yage jsoncpp_lib)

target_include_directories(yage PRIVATE thirdparty/free_image/Source)
target_link_libraries(yage FreeImage)

target_include_directories(yage PRIVATE thirdparty/glm)
target_link_libraries(yage glm)
