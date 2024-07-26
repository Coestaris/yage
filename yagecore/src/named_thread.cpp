/** ========================================================================
 *
 * @file named_thread.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/named_thread.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
std::unordered_map<std::thread::id, std::string>
      yc::named_thread::thread_names {};

/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::string yc::named_thread::get_thread_name(
      std::thread::id id)
{
  auto it = named_thread::thread_names.find(id);
  if (it != named_thread::thread_names.end())
  {
    return it->second;
  }
  return "unknown";
}

yc::named_thread::named_thread(std::string name)
: name(std::move(name))
{
}

std::string yc::named_thread::get_name() const
{
  return name;
}

void yc::named_thread::start()
{
  thread = std::make_unique<std::thread>(&named_thread::start_inplace, this);
  set_port_name();
}

void yc::named_thread::start_inplace()
{
  named_thread::thread_names[std::this_thread::get_id()] = name;
  logging(LOG_INFO, "Thread '%s' started", name.c_str());
  run();
  logging(LOG_INFO, "Thread '%s' finished", name.c_str());
}

void yc::named_thread::join()
{
  if (thread && thread->joinable())
  {
    thread->join();
  }
}