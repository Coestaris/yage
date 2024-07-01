/** ========================================================================
 *
 * @file file_watcher.cpp
 * @date 01-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "core/log.hpp"
#include "core/fs.hpp"

#include <climits>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <csignal>
#include <cstring>

/* Defines ================================================================== */
/* Size of buffer to use when reading inotify events */
#define INOTIFY_BUFFER_SIZE 8192

#define IN_EVENT_DATA_LEN (sizeof(struct inotify_event))
#define IN_EVENT_NEXT(event, length)                                           \
  ((length) -= (event)->len,                                                   \
   (struct inotify_event *)(((char *)(event)) + (event)->len))
#define IN_EVENT_OK(event, length)                                             \
  ((long)(length) >= (long)IN_EVENT_DATA_LEN                                   \
   && (long)(event)->len >= (long)IN_EVENT_DATA_LEN                            \
   && (long)(event)->len <= (long)(length))

/* Macros =================================================================== */
/* Types ==================================================================== */
struct file_watcher_impl
{
  int         inotify_fd = -1;
  int         watch_fd   = -1;
  std::string watched_file;
};

void yage::core::file_watcher::start_watching()
{
  if (watched_files.empty() || watched_files.size() > 1)
  {
    logging(LOG_ERROR, "Invalid number of files to watch");
    return;
  }

  impl              = new file_watcher_impl();
  auto * ctx        = static_cast<file_watcher_impl *>(impl);
  ctx->watched_file = watched_files[0];

  std::string path = watched_files[0];

  ctx->inotify_fd = inotify_init1(IN_NONBLOCK);
  if (ctx->inotify_fd == -1)
  {
    perror("inotify_init1");
    return;
  }

  ctx->watch_fd = inotify_add_watch(
        ctx->inotify_fd, basedir(ctx->watched_file).c_str(), IN_MOVED_TO);
  if (ctx->watch_fd == -1)
  {
    perror("inotify_add_watch");
    close(ctx->inotify_fd);
    ctx->inotify_fd = -1;
    return;
  }
}

bool yage::core::file_watcher::wait_for_changes(uint32_t timeout_ms)
{
  auto * ctx = static_cast<file_watcher_impl *>(impl);

  if (ctx->inotify_fd == -1)
  {
    return false;
  }

  int           poll_timeout = timeout_ms == WAIT_FOREVER ? -1 : timeout_ms;
  struct pollfd fds[1]       = {
    { .fd = ctx->inotify_fd, .events = POLLIN, .revents = 0 },
  };
  int ret = poll(fds, 1, poll_timeout);
  if (ret == -1)
  {
    perror("poll");
    return false;
  }

  if (ret == 0)
  {
    return false;
  }

  if (fds[0].revents & POLLIN)
  {
#define INOTIFY_BUFFER_SIZE 8192
    char buffer[INOTIFY_BUFFER_SIZE];
    int  length = 0;

    /* Read from the FD. It will read all events available up to
     * the given buffer size. */
    if ((length = read(fds[0].fd, buffer, INOTIFY_BUFFER_SIZE)) > 0)
    {
      struct inotify_event * event;

      event = reinterpret_cast<struct inotify_event *>(buffer);

      /* allow c-style cast here */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
      while (IN_EVENT_OK(event, length))
      {
        if (event->wd == ctx->watch_fd)
        {
          /* get filename */
          std::string filename(event->name, event->len);
          filename.shrink_to_fit();
          logging(LOG_INFO, "File %s was modified", filename.c_str());

          /* Using c-style compare, because filename
           * may contain null-terminator */
          if (strcmp(filename.c_str(), basename(ctx->watched_file).c_str())
              == 0)
          {
            return true;
          }

          return false;
        }

        event = IN_EVENT_NEXT(event, length);
      }
#pragma GCC diagnostic pop
    }

    return false;
  }

  return false;
}

void yage::core::file_watcher::stop_watching()
{
  auto * ctx = static_cast<file_watcher_impl *>(impl);

  if (ctx->watch_fd != -1)
  {
    inotify_rm_watch(ctx->inotify_fd, ctx->watch_fd);
    ctx->watch_fd = -1;
  }
  if (ctx->inotify_fd != -1)
  {
    close(ctx->inotify_fd);
    ctx->inotify_fd = -1;
  }

  delete ctx;
}

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
