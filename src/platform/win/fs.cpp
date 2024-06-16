/** ========================================================================
 *
 * @file time.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/fs.hpp"
#include "windows.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yage::core::file_exists(std::string path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

bool yage::core::is_directory(std::string path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (
        dwAttrib != INVALID_FILE_ATTRIBUTES
        && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool yage::core::is_file(std::string path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (
        dwAttrib != INVALID_FILE_ATTRIBUTES
        && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string yage::core::absolute_path(std::string path)
{
  char fullpath[MAX_PATH];
  GetFullPathName(path.c_str(), MAX_PATH, fullpath, nullptr);
  return { fullpath };
}