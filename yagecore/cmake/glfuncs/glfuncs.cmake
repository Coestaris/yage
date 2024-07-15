set(DST_CPP "${PROJECT_DIR}/src/platform/graphics/gl/glfuncs.cpp")
set(DST_HPP "${PROJECT_DIR}/include/yagecore/gl/glfuncs.hpp")
set(SRC_INC "${PROJECT_DIR}/include/yagecore/gl/glfuncs.inc")

# Find python3
find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_Interpreter_FOUND)
    message(FATAL_ERROR "Python3 not found")
endif ()

## Generate glfuncs.hpp
#add_custom_target(glfuncs_hpp
#        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/glfuncs.py
#            --namespace yc::gl --header ${DST_HPP}
#            --input ${SRC_INC}
#)
#add_custom_target(glfuncs_cpp
#        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/glfuncs.py
#            --namespace yc::gl --source ${DST_CPP}
#            --input ${SRC_INC}
#)
#
#add_dependencies(${PROJECT_NAME} glfuncs_hpp glfuncs_cpp)
