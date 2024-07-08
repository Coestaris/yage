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
#include <cstdint>

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class file_watcher
{
  static constexpr uint32_t WAIT_FOREVER = 0xFFFFFFFF;
  static constexpr uint32_t WAIT_NOWAIT  = 0;

private:
  std::vector<std::string> watched_files;

  /* Platform specific implementation */
  void * impl;

  void start_watching();
  void stop_watching();

public:
  file_watcher(std::vector<std::string> files)
  : watched_files(std::move(files))
  {
    start_watching();
  }

  ~file_watcher()
  {
    stop_watching();
  }

  bool wait_for_changes(uint32_t timeout_ms = 0);
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
bool file_exists(const std::string & path);
bool is_directory(const std::string & path);
bool is_file(const std::string & path);

time_t file_mtime(const std::string & path);
time_t file_ctime(const std::string & path);

std::string join_path(const std::string & path1, const std::string & path2);
std::string absolute_path(const std::string & path);
std::string basename(const std::string & path, bool with_extension = true);
std::string basedir(const std::string & path);

std::string get_temp_dir(const std::string & prefix = {});
std::string get_cache_dir(const std::string & prefix = {});

std::vector<std::string> list_files(const std::string & path, const std::string & pattern = "*");

}
