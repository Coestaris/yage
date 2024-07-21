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
  static std::unordered_map<std::thread::id, std::string> thread_names;

  std::unique_ptr<std::thread> thread;
  std::string                  name;

  virtual void run() = 0;

  /* C++ does not provide a way to get the name of
   * the current thread, so we have to set it manually,
   * for each platform. */
  void set_port_name();

public:
  static std::string
  get_thread_name(std::thread::id id = std::this_thread::get_id());

  virtual ~named_thread() = default;
           named_thread(std::string name);

  [[nodiscard]] std::string get_name() const;

  void start();
  void start_inplace();
  void join();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}