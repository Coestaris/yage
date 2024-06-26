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
bool file_exists(const std::string& path);
bool is_directory(const std::string& path);
bool is_file(const std::string& path);

time_t file_mtime(const std::string& path);
time_t file_ctime(const std::string& path);

std::string join_path(std::string path1, std::string path2);
std::string absolute_path(std::string path);
std::string basename(std::string path, bool with_extension = true);

std::string get_temp_dir(const std::string& prefix = {});
std::string get_cache_dir(const std::string& prefix = {});

std::vector<std::string> list_files(const std::string& path, const std::string& pattern = "*");

}
