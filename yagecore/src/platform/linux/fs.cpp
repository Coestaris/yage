/** ========================================================================
 *
 * @file fs.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/fs.hpp"
#include "yagecore/log.hpp"

#include <csignal>
#include <sys/stat.h>
#include <glob.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yc::file_exists(const std::string & path)
{
  if (access(path.c_str(), F_OK) != -1)
  {
    return true;
  }

  return false;
}

bool yc::is_directory(const std::string & path)
{
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
  {
    return false;
  }

  return S_ISDIR(statbuf.st_mode);
}

bool yc::is_file(const std::string & path)
{
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
  {
    return false;
  }

  return S_ISREG(statbuf.st_mode);
}

std::string yc::absolute_path(std::string path)
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

std::string yc::get_temp_dir(const std::string & prefix)
{
  char * temp_dir = getenv("TMPDIR");
  if (temp_dir == nullptr)
  {
    temp_dir = getenv("TMP");
  }

  if (temp_dir == nullptr)
  {
    temp_dir = getenv("TEMP");
  }

  if (temp_dir == nullptr)
  {
    temp_dir = getenv("TEMPDIR");
  }

  if (temp_dir == nullptr)
  {
    temp_dir = const_cast<char *>("/tmp");
  }

  std::string dir
        = prefix.empty() ? ("yage" + std::to_string(getpid())) : prefix;
  dir = std::string(temp_dir) + "/" + dir;

  if (mkdir(dir.c_str(), 0700) == -1 && errno != EEXIST)
  {
    return {};
  }

  return dir;
}

std::string yc::get_cache_dir(const std::string & prefix)
{
  std::string dir;
  auto        tmp = getenv("XDG_CACHE_HOME");
  if (tmp)
  {
    dir = tmp;
  }
  else
  {
    tmp = getenv("HOME");
    if (tmp)
    {
      dir = std::string(tmp) + "/.cache";
    }
    else
    {
      logging(
            LOG_WARN,
            "Using /tmp as cache directory. Cache may be lost on reboot.");
      dir = yc::get_temp_dir(prefix);
    }
  }

  if (!prefix.empty())
  {
    dir += "/" + prefix;
  }
  else
  {
    dir += "/yage";
  }

  if (mkdir(dir.c_str(), 0700) == -1 && errno != EEXIST)
  {
    return {};
  }

  return dir;
}

std::vector<std::string>
yc::list_files(const std::string & path, const std::string & pattern)
{
  std::vector<std::string> files;

  /* glob */
  glob_t glob_result;
  glob((path + "/" + pattern).c_str(), GLOB_TILDE, NULL, &glob_result);

  for (size_t i = 0; i < glob_result.gl_pathc; i++)
  {
    files.emplace_back(glob_result.gl_pathv[i]);
  }

  globfree(&glob_result);

  return files;
}

std::string yc::basename(std::string path, bool with_extension)
{
  size_t pos = path.find_last_of('/');
  if (pos == std::string::npos)
  {
    return path;
  }

  if (with_extension)
  {
    return path.substr(pos + 1);
  }

  size_t dot_pos = path.find_last_of('.');
  if (dot_pos == std::string::npos)
  {
    return path.substr(pos + 1);
  }

  return path.substr(pos + 1, dot_pos - pos - 1);
}

time_t yc::file_mtime(const std::string & path)
{
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
  {
    return 0;
  }

  return statbuf.st_mtime;
}

std::string yc::basedir(std::string path)
{
  size_t pos = path.find_last_of('/');
  if (pos == std::string::npos)
  {
    return path;
  }

  return path.substr(0, pos);
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

  if (path1.back() == '/' && path2.front() == '/')
  {
    return path1 + path2.substr(1);
  }

  if (path1.back() != '/' && path2.front() != '/')
  {
    return path1 + "/" + path2;
  }

  return path1 + path2;
}

time_t yc::file_ctime(const std::string & path)
{
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
  {
    return 0;
  }

  return statbuf.st_ctime;
}