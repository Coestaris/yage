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
class functor
{
private:
  std::function<void()> func;

public:
  functor(std::function<void()> func)
  : func(std::move(func))
  {
  }

  void run()
  {
    func();
  }

  bool has_result()
  {
    return false;
  }

  std::any get_result()
  {
    return true;
  }
};

template <typename FuncRetType, typename... FuncTypes> class worker
{
private:
  std::string name;
  std::any    result;
  bool        void_func;
  bool        completed;
  bool        running;

protected:
  virtual FuncRetType run(FuncTypes... args) = 0;

public:
  worker(const char * name)
  : name(name)
  , void_func(true)
  , completed(false)
  {
  }
  worker(std::string & name)
  : name(name)
  , void_func(std::is_void_v<FuncRetType>)
  , completed(false)
  {
  }

  template <typename... Args> FuncRetType do_work(Args &&... args)
  {
    completed = false;

    if constexpr (std::is_void_v<FuncRetType>)
    {
      run(args...);
      completed = true;
    }
    else
    {
      result    = run(args...);
      completed = true;
      return result;
    }
  }

  std::string get_name() const
  {
    return name;
  }

  void set_running(bool run)
  {
    this->running = run;
  }

  bool is_running()
  {
    return running;
  }
};

class thread_pool
{
private:
  enum class task_status
  {
    in_q,
    completed
  };

  struct task_info
  {
    task_status status = task_status::in_q;
    std::string name;
    std::any    result;
  };

  std::vector<std::thread> threads;

  std::queue<std::pair<functor, uint64_t>> queue;
  std::mutex                               q_mtx;
  std::condition_variable                  q_cv;

  std::unordered_map<uint64_t, task_info>        tasks_info;
  std::unordered_map<std::thread::id, task_info> tasks_info_by_thread;
  std::condition_variable                        tasks_info_cv;
  std::mutex                                     tasks_info_mtx;
  std::mutex                                     tasks_info_by_thread_mtx;

  std::condition_variable wait_all_cv;

  std::atomic<bool>     quite { false };
  std::atomic<uint64_t> last_idx { 0 };
  std::atomic<uint64_t> cnt_completed_tasks { 0 };

  void run(int index);

public:
  thread_pool(const size_t num_threads);
  ~thread_pool();

  template <typename FuncRetType, typename... FuncTypes, typename... Args>
  uint64_t add_task(worker<FuncRetType, FuncTypes...> & worker, Args &&... args)
  {
    log(LOG_DEBUG, "Adding task %s", worker.get_name().c_str());

    const uint64_t task_id = last_idx++;

    std::unique_lock<std::mutex> lock(tasks_info_mtx);
    tasks_info[task_id]      = task_info();
    tasks_info[task_id].name = worker.get_name();
    functor func([&worker, args...]() {
      worker.set_running(true);
      worker.do_work(args...);
      worker.set_running(false);
    });

    lock.unlock();

    std::lock_guard<std::mutex> q_lock(q_mtx);
    queue.emplace(func, task_id);
    q_cv.notify_one();
    return task_id;
  }

  void                    wait(const uint64_t task_id);
  std::any                wait_result(const uint64_t task_id);
  template <class T> void wait_result(const uint64_t task_id, T & value);
  void                    wait_all();
  bool                    calculated(const uint64_t task_id);
  std::string             get_current_task_name();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
void          set_global_thread_pool(thread_pool * pool);
thread_pool * get_global_thread_pool();

}
