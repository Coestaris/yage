cmake_minimum_required(VERSION 3.16)

project(yage LANGUAGES CXX)

function(util_get_filename_component_we file result)
    get_filename_component(filename ${file} NAME)
    string(REPLACE "." ";" name_parts ${filename})
    list(POP_BACK name_parts)
    list(JOIN name_parts "." name)
    set(${result} "${name}" PARENT_SCOPE)
endfunction()

function(util_add_short_filename_property)
    foreach (file ${ARGV})
        get_source_file_property(__props ${file} COMPILE_DEFINITIONS)
        util_get_filename_component_we("${file}" filename)
        set_source_files_properties(
                ${file} PROPERTIES COMPILE_DEFINITIONS
                "${__props};__SHORT_FILE__=\"${filename}\";__USE_SHORT_FILE__=1"
        )
    endforeach ()
endfunction()

# Apply the short filename property to .cpp/.hpp files
file(GLOB_RECURSE ___src *.cpp *.hpp)
util_add_short_filename_property(${___src})

add_subdirectory(yagecore)
add_subdirectory(yagepack)
add_subdirectory(app)

