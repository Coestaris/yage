/** ========================================================================
 *
 * @file file_watcher.cpp
 * @date 01-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <chrono>
#include <thread>
#include <windows.h>
#include "yagecore/log.hpp"
#include "yagecore/fs.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class file_watcher_impl : public yc::file_watcher::impl
{
public:
  std::string watched_file {};
  HANDLE      dir_handle {};
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::file_watcher::start_watching()
{
  if (watched_files.empty() || watched_files.size() > 1)
  {
    logging(LOG_ERROR, "Invalid number of files to watch");
    return;
  }

  auto ctx = std::make_shared<file_watcher_impl>();
  pimpl    = std::dynamic_pointer_cast<impl>(ctx);

  ctx->watched_file = watched_files[0];
  std::string dir   = basedir(ctx->watched_file);

  ctx->dir_handle = CreateFile(
        dir.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr);
  if (ctx->dir_handle == INVALID_HANDLE_VALUE)
  {
    logging(LOG_ERROR, "Failed to open directory: %lu", GetLastError());
    return;
  }
}

bool yc::file_watcher::wait_for_changes(uint32_t timeout_ms)
{
  auto ctx = std::dynamic_pointer_cast<file_watcher_impl>(pimpl);

  if (ctx->dir_handle == INVALID_HANDLE_VALUE)
  {
    logging(LOG_ERROR, "Invalid directory handle");
    return false;
  }

  FILE_NOTIFY_INFORMATION notify_info[1024];
  DWORD                   bytes_returned = 0;

  OVERLAPPED overlapped;
  if (!ReadDirectoryChangesW(
            ctx->dir_handle, notify_info, sizeof(notify_info), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
                  | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
                  | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
            &bytes_returned, &overlapped, nullptr))
  {
    logging(LOG_ERROR, "Failed to read directory changes: %lu", GetLastError());
    return false;
  }

  /* Wait for events */
  if (WaitForSingleObject(ctx->dir_handle, timeout_ms) != WAIT_OBJECT_0)
  {
    /* Silently ignore timeout */
    return false;
  }

  /* Get the changes */
  bytes_returned = 0;
  if (!GetOverlappedResult(
            ctx->dir_handle, &overlapped, &bytes_returned, FALSE))
  {
    logging(LOG_ERROR, "Failed to get overlapped result: %lu", GetLastError());
    return false;
  }

  if (bytes_returned == 0)
  {
    logging(LOG_ERROR, "No bytes returned");
    return false;
  }

  auto * notify = notify_info;

  do
  {
    std::wstring file_name(notify->FileName, notify->FileNameLength / 2);
    std::string  file_name_utf8(file_name.begin(), file_name.end());

    if (file_name_utf8 == basename(ctx->watched_file))
    {
      return true;
    }

    notify = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(
          reinterpret_cast<char *>(notify) + notify->NextEntryOffset);

  } while (notify->NextEntryOffset != 0);

  return false;
}

void yc::file_watcher::stop_watching()
{
  auto ctx = std::dynamic_pointer_cast<file_watcher_impl>(pimpl);
  CloseHandle(ctx->dir_handle);
}
