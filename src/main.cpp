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
#include <utility>
#include "core/log.hpp"
#include "core/pubsub.hpp"
#include "core/threads.hpp"
#include "core/os_events.hpp"

#include "platform/graphics/platform.hpp"
#include "core/time.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class frequency_keeper
{
private:
  size_t calls        = 0;
  float  prev_ms      = 0.0f;
  float  prev_call_ms = 0.0f;
  float  cps          = 0.0f;

  float desired_interval;

public:
  explicit frequency_keeper(size_t max_cps = 60)
  {
    desired_interval = 1000.0f / max_cps;
  }

  float call()
  {
    calls++;
    float ms              = yage::core::get_ms();
    float prev_call_delta = ms - prev_call_ms;
    prev_call_ms          = ms;

    if (prev_call_delta < desired_interval)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(desired_interval - prev_call_delta)));
    }

    float delta = ms - prev_ms;

    if (delta > 1000.0f)
    {
      cps     = calls / delta * 1000.0f;
      calls   = 0;
      prev_ms = ms;
    }

    return prev_call_delta;
  }

  float get_cps() const
  {
    return cps;
  }
};

class renderer_worker : public yage::core::worker<void>,
                        public yage::core::signal_subscriber
{
private:
  std::string              win_title;
  yage::graphics::geometry win_geometry;
  yage::graphics::window * window;

  float draw_time = 0.0f;

public:
  frequency_keeper keeper;

  void on_event(const yage::core::event<int> & event) override
  {
    log(LOG_DEBUG, "Received stop signal: %d. Closing window", event.getData());
    window->close();
  }

  void run() override
  {
    if (window->open(win_title, win_geometry, false) != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to open window");
      return;
    }

    yage::graphics::graphics * graphics = window->get_graphics();

    while (true)
    {
      process_events();

      float frame_interval = keeper.call();

      if (!window->is_open())
      {
        log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
        break;
      }

      float draw_start = yage::core::get_ms();
      graphics->clear();
      graphics->draw_rect(0, 0, 800, 600);

      if (window->update() != yage::core::error::OK)
      {
        log(LOG_ERROR, "Failed to update window");
        break;
      }

      /* perform moving average */
      draw_time = draw_time * 0.9f + (yage::core::get_ms() - draw_start) * 0.1f;
    }
  }

  float get_draw_time() const
  {
    return draw_time;
  }

public:
  renderer_worker(
        yage::graphics::window * window,
        std::string              title,
        yage::graphics::geometry geometry,
        int                      max_fps)
  : yage::core::worker<void>("renderer")
  , win_title(std::move(title))
  , win_geometry(geometry)
  , window(window)
  , keeper(max_fps)
  {
  }
};

class statistic_worker : public yage::core::worker<void>
{
private:
  renderer_worker * renderer;

public:
  explicit statistic_worker(renderer_worker * renderer)
  : yage::core::worker<void>("statistic")
  , renderer(renderer)
  {
  }

  void run() override
  {
    while (true)
    {
      if (!renderer->is_running())
      {
        log(LOG_DEBUG,
            "Renderer worker is not running. Exiting statistic worker");
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
      log(LOG_INFO, "Draw time: %.2f ms, FPS: %.2f", renderer->get_draw_time(),
          renderer->keeper.get_cps());
    }
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
int main(int argc, char * argv[])
{
  yage::core::os_events events;

  yage::core::stdout_logger logger;
  logger.set_level(LOG_DEBUG);
  set_global_logger(&logger);

  yage::core::thread_pool pool(3);
  set_global_thread_pool(&pool);

  yage::graphics::settings settings;
  yage::graphics::window * window
        = yage::platform::graphics::create_window(settings);

  std::string              title    = "Hello, World!";
  yage::graphics::geometry geometry = { 0, 0, 800, 600 };
  int                      max_fps  = 60;
  renderer_worker          renderer(window, title, geometry, max_fps);

  pool.add_task(renderer);
  events.on_stop_signal.subscribe(renderer);

  statistic_worker statistic(&renderer);
  pool.add_task(statistic);

  pool.wait_all();

  return 0;
}