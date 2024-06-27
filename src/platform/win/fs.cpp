/** ========================================================================
 *
 * @file time.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <stdexcept>
#include "core/fs.hpp"
#include "windows.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yage::core::file_exists(const std::string & path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

bool yage::core::is_directory(const std::string & path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (
        dwAttrib != INVALID_FILE_ATTRIBUTES
        && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool yage::core::is_file(const std::string & path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (
        dwAttrib != INVALID_FILE_ATTRIBUTES
        && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

time_t yage::core::file_mtime(const std::string & path)
{
  throw std::runtime_error("Not implemented");
  return {};
}

time_t yage::core::file_ctime(const std::string & path)
{
  throw std::runtime_error("Not implemented");
  return {};
}

std::string yage::core::join_path(std::string path1, std::string path2)
{
  throw std::runtime_error("Not implemented");
  return {};
}

std::string yage::core::absolute_path(std::string path)
{
  char fullpath[MAX_PATH];
  GetFullPathName(path.c_str(), MAX_PATH, fullpath, nullptr);
  return { fullpath };
}

std::string yage::core::basename(std::string path, bool with_extension)
{
  throw std::runtime_error("Not implemented");
  return {};
}

std::string yage::core::get_temp_dir(const std::string & prefix)
{
  throw std::runtime_error("Not implemented");
  return {};
}

std::string yage::core::get_cache_dir(const std::string & prefix)
{
  throw std::runtime_error("Not implemented");
  return {};
}

std::vector<std::string>
yage::core::list_files(const std::string & path, const std::string & pattern)
{
  throw std::runtime_error("Not implemented");
  return {};
}