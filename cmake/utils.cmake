#
# @brief Returns filename without extensions
#
# Example:
#   file.c      => file
#   file_V1.3.c => file_V1.3
#
# @param[in]  file   String to process
# @param[out] result Name of variable to put processing result
#
function(util_get_filename_component_we file result)
    get_filename_component(filename ${file} NAME)
    string(REPLACE "." ";" name_parts ${filename})
    list(POP_BACK name_parts)
    list(JOIN name_parts "." name)
    set(${result} "${name}" PARENT_SCOPE)
endfunction()

#
# @brief Defines two macros for usage with syslog module
#   - #define __USE_SHORT_FILE__ 1 // for usage inside `syslog()`
#   - #define __SHORT_FILE__ "short_file_name..."
#
# @note this function destructively declares those macros for each file with its filename
#
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
