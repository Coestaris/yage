/** ========================================================================
 *
 * @file time.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <stdexcept>
#include "yagecore/fs.hpp"
#include "windows.h"
#include "yagecore/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yc::file_exists(const std::string & path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

bool yc::is_directory(const std::string & path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (
        dwAttrib != INVALID_FILE_ATTRIBUTES
        && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool yc::is_file(const std::string & path)
{
  DWORD dwAttrib = GetFileAttributes(path.c_str());

  return (
        dwAttrib != INVALID_FILE_ATTRIBUTES
        && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

time_t yc::file_mtime(const std::string & path)
{
  throw std::runtime_error("Not implemented");
  return {};
}

time_t yc::file_ctime(const std::string & path)
{
  throw std::runtime_error("Not implemented");
  return {};
}

std::string yc::join_path(std::string path1, std::string path2)
{
  if (path1.empty())
  {
    return path2;
  }

  if (path2.empty())
  {
    return path1;
  }

  if (path1.back() == '\\' || path1.back() == '/')
  {
    path1.pop_back();
  }

  if (path2.front() == '\\' || path2.front() == '/')
  {
    path2.erase(0, 1);
  }

  return path1 + "\\" + path2;
}

std::string yc::absolute_path(std::string path)
{
  char fullpath[MAX_PATH];
  GetFullPathName(path.c_str(), MAX_PATH, fullpath, nullptr);
  return { fullpath };
}

std::string yc::basedir(std::string path)
{
  /* Find the last slash */
  size_t last_slash = path.find_last_of("\\/");
  if (last_slash == std::string::npos)
  {
    return path;
  }

  return path.substr(0, last_slash);
}

std::string yc::basename(std::string path, bool with_extension)
{
  /* Find the last slash */
  size_t last_slash = path.find_last_of("\\/");
  if (last_slash == std::string::npos)
  {
    return path;
  }

  /* Find the last dot */
  if (with_extension)
  {
    return path.substr(last_slash + 1);
  }
  else
  {
    size_t last_dot = path.find_last_of(".");
    if (last_dot == std::string::npos)
    {
      return path.substr(last_slash + 1);
    }
    else
    {
      return path.substr(last_slash + 1, last_dot - last_slash - 1);
    }
  }
}

std::string yc::get_temp_dir(const std::string & prefix)
{
  char temp_dir[MAX_PATH];
  GetTempPath(MAX_PATH, temp_dir);

  /* append default prefix, if not provided */
  std::string temp_dir_str = temp_dir;
  if (!prefix.empty())
  {
    temp_dir_str = join_path(temp_dir_str, prefix);
  }
  else
  {
    temp_dir_str = join_path(temp_dir_str, "yage");
  }

  /* create directory, if not exists */
  if (!yc::is_directory(temp_dir_str))
  {
    if (!CreateDirectory(temp_dir_str.c_str(), nullptr))
    {
      logging(
            LOG_ERROR, "Failed to create temp directory: %s",
            temp_dir_str.c_str());
      return {};
    }
  }

  logging(LOG_DEBUG, "Temp directory: %s", temp_dir_str.c_str());
  return temp_dir_str;
}

std::string yc::get_cache_dir(const std::string & prefix)
{
  /* Store cache in the %APPDATA% directory */
  char * appdata = getenv("APPDATA");

  /* append default prefix, if not provided */
  std::string cache_dir = appdata;
  if (!prefix.empty())
  {
    cache_dir = join_path(cache_dir, prefix);
  }
  else
  {
    cache_dir = join_path(cache_dir, "yage");
  }

  /* create directory, if not exists */
  if (!yc::is_directory(cache_dir))
  {
    if (!CreateDirectory(cache_dir.c_str(), nullptr))
    {
      logging(
            LOG_ERROR, "Failed to create cache directory: %s",
            cache_dir.c_str());
      return {};
    }
  }

  logging(LOG_DEBUG, "Cache directory: %s", cache_dir.c_str());
  return cache_dir;
}

std::vector<std::string>
yc::list_files(const std::string & path, const std::string & pattern)
{
  std::vector<std::string> files;

  WIN32_FIND_DATA findFileData;
  HANDLE hFind = FindFirstFile((path + "\\" + pattern).c_str(), &findFileData);
  if (hFind == INVALID_HANDLE_VALUE)
  {
    logging(LOG_ERROR, "Failed to list files in directory: %s", path.c_str());
    return files;
  }

  do
  {
    files.push_back(join_path(path, findFileData.cFileName));
  } while (FindNextFile(hFind, &findFileData) != 0);

  FindClose(hFind);

  return files;
}