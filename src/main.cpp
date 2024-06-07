/** ========================================================================
 *
 * @file main.cpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <thread>
#include <csignal>
#include "core/log.hpp"
#include "core/pubsub.hpp"
#include "core/threads.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class Worker1 : public yage::core::sync_subscriber<char>,
                public yage::core::worker
{
private:
  void on_event(const yage::core::event<char> & event) override
  {
    log(LOG_DEBUG, "Worker1 Received event: %d", event.getData());
  }

  void run() override
  {
    for (size_t i = 0; i < 3; i++)
    {
      log(LOG_DEBUG, "Worker1 is working");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

public:
  Worker1()
  : worker("Worker1")
  {
  }
};

class Worker2 : public yage::core::subscriber<char>, public yage::core::worker
{
private:
  void on_event(const yage::core::event<char> & event) override
  {
    log(LOG_DEBUG, "Worker2 Received event: %d", event.getData());
  }

  void run() override
  {
    for (size_t i = 0; i < 2; i++)
    {
      log(LOG_DEBUG, "Worker2 is working");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

public:
  Worker2()
  : worker("Worker2")
  {
  }
};

class Worker3 : public yage::core::publisher<char>, public yage::core::worker
{
  void run() override
  {
    for (size_t i = 0; i < 5; i++)
    {
      log(LOG_DEBUG, "Publisher is working");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

public:
  Worker3()
  : worker("Publisher")
  {
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
int main(int argc, char * argv[])
{
  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  log(LOG_INFO, "Starting application");

  yage::core::thread_pool pool(3);
  set_global_thread_pool(&pool);

  Worker1 worker1;
  Worker2 worker2;
  Worker3 worker3;

  pool.enqueue(worker1);
  pool.enqueue(worker2);
  pool.enqueue(worker3);

  pool.join();

  return 0;
}