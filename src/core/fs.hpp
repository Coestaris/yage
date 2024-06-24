/** ========================================================================
*
* @file fs.hpp
* @date 16-06-2024
* @author Maxim Kurylko <vk_vm@ukr.net>
*
*  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <vector>

namespace yage::core
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
bool file_exists(std::string path);
bool is_directory(std::string path);
bool is_file(std::string path);

std::string absolute_path(std::string path);
std::string basename(std::string path, bool with_extension = true);
std::string get_temp_dir(std::string prefix = {});
std::vector<std::string> list_files(std::string path, std::string pattern="*");

}
