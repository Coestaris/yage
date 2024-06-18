/** ========================================================================
 *
 * @file threads.hpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <any>
#include <atomic>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <map>
#include <string>

#include "log.hpp"

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
using worker_id = uint64_t;

worker_id new_worker_id();

class worker_info
{
public:
  std::string name;
  bool        running;
  bool        done;
  worker_id   id;
};

template <typename... FuncTypes> class worker
{
protected:
  virtual void run(FuncTypes... args) = 0;

public:
  worker_info info;

  worker(const char * name)
  {
    info.name = name;
    info.id   = new_worker_id();
  }

  worker(std::string & name)
  {
    info.name = name;
    info.id   = new_worker_id();
  }

  template <typename... Args> void do_work(Args &&... args)
  {
    log(LOG_DEBUG, "Running task %s", info.name.c_str());
    run(args...);
    log(LOG_DEBUG, "Task %s completed", info.name.c_str());
  }

  std::string get_name() const
  {
    return info.name;
  }

  bool is_running()
  {
    return this->running;
  }
};

class thread_pool
{
private:
  std::vector<std::thread> threads;

  std::queue<std::pair<std::function<void()>, yage::core::worker_info>> queue;

  std::mutex              q_mtx;
  std::condition_variable q_cv;

  std::unordered_map<worker_id, yage::core::worker_info>       info;
  std::unordered_map<std::thread::id, yage::core::worker_info> info_by_thread;

  std::condition_variable tasks_info_cv;
  std::mutex              tasks_info_mtx;
  std::mutex              tasks_info_by_thread_mtx;

  std::condition_variable wait_all_cv;

  std::atomic<bool>      quite { false };
  std::atomic<worker_id> cnt_completed_tasks { 0 };

  void run(int index);

public:
  thread_pool(const size_t num_threads);
  ~thread_pool();

  template <typename FuncRetType, typename... FuncTypes, typename... Args>
  worker_id
  add_task(worker<FuncRetType, FuncTypes...> & worker, Args &&... args)
  {
    log(LOG_DEBUG, "Adding task %s", worker.get_name().c_str());

    std::unique_lock<std::mutex> lock(tasks_info_mtx);
    info[worker.info.id] = worker.info;

    lock.unlock();

    std::lock_guard<std::mutex> q_lock(q_mtx);
    queue.emplace(
          std::bind(
                &yage::core::worker<FuncRetType, FuncTypes...>::do_work,
                &worker, args...),
          std::ref(worker.info));
    q_cv.notify_one();

    return worker.info.id;
  }

  void        wait(worker_id task_id);
  void        wait_all();
  std::string get_current_task_name();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
void          set_global_thread_pool(thread_pool * pool);
thread_pool * get_global_thread_pool();

}
