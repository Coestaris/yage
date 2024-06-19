/** ========================================================================
 *
 * @file event_stream.hpp
 * @date 19-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <queue>
#include <mutex>
#include "core/log.hpp"

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
template <typename T> class event_stream_listener
{
  size_t max_queue_size = 100;
  std::string name{};

  std::queue<T> queue;
  std::mutex    mutex;

public:
  void push(T data)
  {
    std::lock_guard<std::mutex> lock(mutex);

    if (queue.size() >= max_queue_size)
    {
      log(LOG_WARN, "%s: queue is full, dropping data", name.c_str());
      return;
    }

    queue.push(data);
    }

  bool get_next(T & data)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty())
    {
      return false;
    }
    data = queue.front();
    queue.pop();
    return true;
  }
};

template <typename T> class event_stream
{
  std::vector<event_stream_listener<T> *> listeners;

public:
  void push(T data)
  {
    for (auto listener : listeners)
    {
      listener->push(data);
    }
  }

  void add_listener(event_stream_listener<T> * listener)
  {
    listeners.push_back(listener);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
