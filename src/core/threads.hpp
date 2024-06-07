/** ========================================================================
 *
 * @file threads.hpp
 * @date 07-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <map>
#include <string>
#include "log.hpp"

namespace yage::core
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class worker
{
private:
  std::string name;

public:
  worker(std::string & name)
  : name(name)
  {
  }
  worker(const char * name)
  : name(name)
  {
  }

  virtual ~worker() = default;

  virtual void run() = 0;

  std::string get_name() const
  {
    return name;
  }
};

/* Class that represents a simple thread pool
 * Took an idea from: https://www.geeksforgeeks.org/thread-pool-in-cpp/ */
class thread_pool
{

private:
  /* Vector to store worker threads */
  std::vector<std::thread> threads;

  std::map<std::thread::id, std::string> names;

  /* Queue of tasks */
  std::queue<std::tuple<std::function<void()>, std::string>> tasks;

  /* Mutex to synchronize access to shared data */
  std::mutex queue_mutex;

  /* Condition variable to signal changes in the state of the tasks queue */
  std::condition_variable cv;

  std::thread::id root_thread_id = std::this_thread::get_id();

  /* Flag to indicate whether the thread pool should stop or not */
  bool stop = false;

public:
  /* Constructor to create a thread pool with a given number of threads */
  thread_pool(size_t num_threads = std::thread::hardware_concurrency())
  {
    /* Creating worker threads */
    for (size_t i = 0; i < num_threads; ++i)
    {
      threads.emplace_back([this]() -> void {
        while (true)
        {
          std::function<void()> task;
          std::string           name;

          /* The reason for putting the below code here is to unlock the queue
           * before executing the task so that other threads can perform
           * enqueue tasks */
          {
            /* Locking the queue so that data can be shared safely */
            std::unique_lock<std::mutex> lock(queue_mutex);

            /* Waiting until there is a task to execute or pool is stopped */
            cv.wait(lock, [this] { return !tasks.empty() || stop; });

            /* exit the thread in case the pool is
             * stopped and there are no tasks */
            if (stop && tasks.empty())
            {
              return;
            }

            /* Get the next worker from the queue */
            auto tuple = tasks.front();
            task       = std::move(std::get<0>(tuple));
            name       = std::get<1>(tuple);
            tasks.pop();

            names[std::this_thread::get_id()] = name;
          }

          task();

          {
            /* Locking the queue so that data can be shared safely */
            std::unique_lock<std::mutex> lock(queue_mutex);

            names.erase(std::this_thread::get_id());
          }
        }
      });
    }
  }

  /* Destructor to stop the thread pool */
  ~thread_pool()
  {
    join();
  }

  std::string get_name()
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    auto                         id = std::this_thread::get_id();
    if (id == root_thread_id)
    {
      return "root";
    }
    else if (names.find(id) != names.end())
    {
      return names[id];
    }

    return "???";
  }

  void join()
  {
    {
      /* Lock the queue to update the stop flag safely */
      std::unique_lock<std::mutex> lock(queue_mutex);
      stop = true;
    }

    /* Notify all threads */
    cv.notify_all();

    /* joining all worker threads to ensure they have completed their tasks */
    for (auto & thread : threads)
    {
      if (thread.joinable())
        thread.join();
    }
  }

  void enqueue(worker & worker)
  {
    log(LOG_DEBUG, "Enqueue task %s", worker.get_name().c_str());

    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      if (!tasks.empty())
      {
        log(LOG_WARN, "Task queue is not empty: %lu", tasks.size());
      }

      std::function<void()> task_wrapper = [&worker]() -> void {
        log(LOG_INFO, "Task %s is running", worker.get_name().c_str());
        worker.run();
        log(LOG_INFO, "Task %s is done", worker.get_name().c_str());
      };

      tasks.emplace(std::move(task_wrapper), worker.get_name());
    }

    cv.notify_one();
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
void                         set_global_thread_pool(thread_pool * pool);
std::shared_ptr<thread_pool> get_global_thread_pool();

}
