/** ========================================================================
 *
 * @file threads.hpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <algorithm>
#include <string>
#include <thread>
#include <unordered_map>
#include "yagecore/log.hpp"

namespace yc::util
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class named_thread
{
private:
  static std::unordered_map<std::thread::id, std::string> thread_names;

  std::unique_ptr<std::thread> thread;
  std::string                  name;

  virtual void run() = 0;

  /* C++ does not provide a way to get the name of
   * the current thread, so we have to set it manually,
   * for each platform. */
  void set_port_name();

public:
  static inline std::string
  get_thread_name(std::thread::id id = std::this_thread::get_id())
  {
    auto it = named_thread::thread_names.find(id);
    if (it != named_thread::thread_names.end())
    {
      return it->second;
    }
    return "unknown";
  }

  named_thread(std::string name)
  : name(std::move(name))
  {
  }

  std::string get_name() const
  {
    return name;
  }

  void start()
  {
    thread = std::make_unique<std::thread>(&named_thread::start_inplace, this);
    set_port_name();
  }

  void start_inplace()
  {
    named_thread::thread_names[std::this_thread::get_id()] = name;
    logging(LOG_INFO, "Thread '%s' started", name.c_str());
    run();
    logging(LOG_INFO, "Thread '%s' finished", name.c_str());
  }

  void join()
  {
    if (thread && thread->joinable())
    {
      thread->join();
    }
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}