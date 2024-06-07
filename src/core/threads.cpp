/** ========================================================================
 *
 * @file threads.cpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "threads.hpp"
#include <stdexcept>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static struct
{
  yage::core::thread_pool * pool;

} ctx = { .pool = nullptr };

/* Shared functions ========================================================= */
void yage::core::set_global_thread_pool(yage::core::thread_pool * pool)
{
  ctx.pool = pool;
}

yage::core::thread_pool * yage::core::get_global_thread_pool()
{
  //  if (!ctx.pool)
  //    throw std::runtime_error("Thread pool is not set");

  return ctx.pool;
}

yage::core::thread_pool::thread_pool(const size_t num_threads)
{
  threads.reserve(num_threads);
  for (size_t i = 0; i < num_threads; ++i)
  {
    threads.emplace_back(&thread_pool::run, this, i);
  }
}

yage::core::thread_pool::~thread_pool()
{
  quite = true;
  q_cv.notify_all();
  for (size_t i = 0; i < threads.size(); ++i)
  {
    threads[i].join();
  }
}

void yage::core::thread_pool::run(int index)
{
  while (!quite)
  {
    std::unique_lock<std::mutex> lock1(q_mtx);
    q_cv.wait(lock1, [this]() -> bool { return !queue.empty() || quite; });

    std::pair<functor, uint64_t> task = std::move(queue.front());

    if (!queue.empty() && !quite)
    {
      {
        std::lock_guard<std::mutex> lock2(tasks_info_by_thread_mtx);
        tasks_info_by_thread[std::this_thread::get_id()]
              = tasks_info[task.second];
      }

      queue.pop();
      lock1.unlock();

      log(LOG_DEBUG, "worker %d is running task %s", index,
          tasks_info[task.second].name.c_str());
      task.first.run();
      log(LOG_DEBUG, "worker %d finished task %s", index,
          tasks_info[task.second].name.c_str());

      {
        std::lock_guard<std::mutex> lock2(tasks_info_mtx);
        if (task.first.has_result())
        {
          tasks_info[task.second].result = task.first.get_result();
        }
        tasks_info[task.second].status = task_status::completed;
      }

      {
        std::lock_guard<std::mutex> lock2(tasks_info_by_thread_mtx);
        tasks_info_by_thread.erase(std::this_thread::get_id());
      }

      ++cnt_completed_tasks;
    }
    wait_all_cv.notify_all();
    tasks_info_cv.notify_all(); // notify for wait function
  }
}

void yage::core::thread_pool::wait(const uint64_t task_id)
{
  std::unique_lock<std::mutex> lock(tasks_info_mtx);
  tasks_info_cv.wait(lock, [this, task_id]() -> bool {
    return task_id < last_idx
           && tasks_info[task_id].status == task_status::completed;
  });
}

std::any yage::core::thread_pool::wait_result(const uint64_t task_id)
{
  std::unique_lock<std::mutex> lock(tasks_info_mtx);
  tasks_info_cv.wait(lock, [this, task_id]() -> bool {
    return task_id < last_idx
           && tasks_info[task_id].status == task_status::completed;
  });
  return tasks_info[task_id].result;
}

template <class T>
void yage::core::thread_pool::wait_result(const uint64_t task_id, T & value)
{
  std::unique_lock<std::mutex> lock(tasks_info_mtx);
  tasks_info_cv.wait(lock, [this, task_id]() -> bool {
    return task_id < last_idx
           && tasks_info[task_id].status == task_status::completed;
  });
  value = std::any_cast<T>(tasks_info[task_id].result);
}

void yage::core::thread_pool::wait_all()
{
  std::unique_lock<std::mutex> lock(tasks_info_mtx);
  wait_all_cv.wait(
        lock, [this]() -> bool { return cnt_completed_tasks == last_idx; });
}

bool yage::core::thread_pool::calculated(const uint64_t task_id)
{
  std::lock_guard<std::mutex> lock(tasks_info_mtx);
  return task_id < last_idx
         && tasks_info[task_id].status == task_status::completed;
}

std::string yage::core::thread_pool::get_current_task_name()
{
  std::lock_guard<std::mutex> lock(tasks_info_by_thread_mtx);
  if (tasks_info_by_thread.find(std::this_thread::get_id())
      == tasks_info_by_thread.end())
  {
    return "unknown";
  }

  return tasks_info_by_thread[std::this_thread::get_id()].name;
}
