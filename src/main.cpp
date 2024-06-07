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
                public yage::core::worker<void>
{
public:
  void on_event(const yage::core::event<char> & event) override
  {
    log(LOG_DEBUG, "Worker1 Received event: %d", event.getData());
  }

  void run() override
  {
    while (true)
    {
      process_events();
    }
  }

public:
  Worker1()
  : worker("Worker1")
  {
  }
};

class Worker2 : public yage::core::subscriber<char>,
                public yage::core::worker<void>
{
public:
  void on_event(const yage::core::event<char> & event) override
  {
    log(LOG_DEBUG, "Worker2 Received event: %d", event.getData());
  }

  void run() override
  {
    while (true)
      ;
  }

public:
  Worker2()
  : worker("Worker2")
  {
  }
};

class Parent : public yage::core::worker<void, int>
{
private:
  yage::core::publisher<char> mouse_moved_pub;
  yage::core::publisher<char> key_entered_pub;

public:
  void run(int arg) override
  {
    while (1)
    {
      /* Read non-blocking from stdin */
      char c = getchar();
      if (c != EOF)
      {
        log(LOG_DEBUG, "Worker3 is publishing event: %d", c);
        mouse_moved_pub.publish(yage::core::event<char>(c));
        key_entered_pub.publish(yage::core::event<char>(c + 1));
      }
    }
  }

  yage::core::publisher<char> & get_mouse_moved_pub()
  {
    return mouse_moved_pub;
  }

  yage::core::publisher<char> & get_key_entered_pub()
  {
    return key_entered_pub;
  }

public:
  Parent()
  : worker("Parent")
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

  Parent parent;
  Worker1 w1;
  Worker2 w2;

  parent.get_mouse_moved_pub().subscribe(w1);
  parent.get_key_entered_pub().subscribe(w2);

  pool.add_task(w1);
  pool.add_task(w2);
  pool.add_task(parent, 400);

  pool.wait_all();

  return 0;
}