/** ========================================================================
 *
 * @file fs.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *  ========================================================================= */

/* Includes ================================================================= */
#include <csignal>
#include <sys/stat.h>
#include "core/fs.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yage::core::file_exists(std::string path)
{
  if (access(path.c_str(), F_OK) != -1)
  {
    return true;
  }

  return false;
}

bool yage::core::is_directory(std::string path)
{
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
  {
    return false;
  }

  return S_ISDIR(statbuf.st_mode);
}

bool yage::core::is_file(std::string path)
{
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
  {
    return false;
  }

  return S_ISREG(statbuf.st_mode);
}

std::string yage::core::absolute_path(std::string path)
{
  char * resolved_path = realpath(path.c_str(), nullptr);
  if (resolved_path == nullptr)
  {
    return path;
  }

  std::string result(resolved_path);
  free(resolved_path);

  return result;
}
