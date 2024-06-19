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
#include "core/log.hpp"

namespace yage::core::util
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class named_thread
{
private:
  static std::unordered_map<std::thread::id, std::string> thread_names;

  std::thread * thread = nullptr;
  std::string   name;
  virtual void  run() = 0;

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

  virtual ~named_thread()
  {
    delete thread;
  }

  const std::string & get_name() const
  {
    return name;
  }

  void start()
  {
    thread = new std::thread(&named_thread::start_inplace, this);
  }

  void start_inplace()
  {
    named_thread::thread_names[std::this_thread::get_id()] = name;
    log(LOG_INFO, "Thread '%s' started", name.c_str());
    run();
    log(LOG_INFO, "Thread '%s' finished", name.c_str());
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