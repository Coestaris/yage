cmake_minimum_required(VERSION 3.16)

separate_arguments(YAGE_CXX_COMPILE_FLAGS UNIX_COMMAND ${YAGE_CXX_COMPILE_FLAGS})
separate_arguments(YAGE_CXX_LINK_FLAGS UNIX_COMMAND ${YAGE_CXX_LINK_FLAGS})

if (WIN32)
    set(SHARED_EXTENTION ".dll")
    set(EXECUTABLE_EXTENTION ".exe")
else ()
    set(SHARED_EXTENTION ".so")
    set(EXECUTABLE_EXTENTION "")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(yage LANGUAGES CXX)

if (REQUEST_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supported OUTPUT error)
    if (supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(WARNING "IPO is not supported: ${error}")
    endif ()
endif ()

add_subdirectory(yagecore)
add_subdirectory(yagepack)
add_subdirectory(app)

