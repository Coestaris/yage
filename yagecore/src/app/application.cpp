/** ========================================================================
 *
 * @file threads.cpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "../../include/yagecore/app/application.hpp"
#include "../../include/yagecore/app/threads/renderer.hpp"
#include "../../include/yagecore/app/threads/logic.hpp"
#include "../../include/yagecore/util/sync_point.hpp"
#include "../../include/yagecore/util/profiler.hpp"
#include "../../include/yagecore/error.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::app::application::render_data::render_data(
      std::shared_ptr<yc::app::asset_factory> asset_factory,
      std::shared_ptr<yc::graphics::window>   window)
: asset_factory(std::move(asset_factory))
, window(std::move(window)) {};

yc::app::application::application(
      std::shared_ptr<yc::app::asset_factory>       asset_factory,
      const std::shared_ptr<yc::graphics::window> & window,
      std::shared_ptr<yc::os_events>                os_events)
: os_events(std::move(os_events))
, window(window)
, render_data(std::move(asset_factory), window)
{
  input_mgr = std::make_shared<input_manager>(this->window);
}

void yc::app::application::stop()
{
  render_data.window->close();
}

void yc::app::application::add_object(std::shared_ptr<yc::app::object> object)
{
  std::lock_guard<std::mutex> lock(logic_data.object_to_add_mutex);
  logic_data.object_to_add.push_back(std::move(object));
}

void yc::app::application::destroy_object(
      const std::shared_ptr<yc::app::object> & object, bool hard)
{
  if (hard)
  {
    auto it = std::find(
          common_data.objects.begin(), common_data.objects.end(), object);
    if (it != common_data.objects.end())
    {
      common_data.objects.erase(it);
    }
  }
  else
  {
    object->logic_data.should_be_deleted = true;
  }
}

void yc::app::application::destroy_all_objects(bool hard)
{
  for (auto & object : common_data.objects)
  {
    object->logic_data.should_be_deleted = true;
  }

  if (hard)
  {
    common_data.objects.clear();
  }
}

[[nodiscard]] yc::graphics::geometry yc::app::application::get_geometry() const
{
  yc::graphics::geometry geometry {};
  render_data.window->get_geometry(geometry);
  return geometry;
}

std::shared_ptr<yc::app::input_manager> yc::app::application::get_input()
{
  return input_mgr;
}

yc::error yc::app::application::start()
{
  /*
   * Threading model:
   *           Sync                                     Sync
   * Renderer:   ||  Start drawing scene   |              || Copy objects data
   * Logic:      ||  Process input | Update objects | etc ||
   */
  auto renderer_profiler = std::make_shared<yc::util::simple_profiler>();
  auto renderer_thread   = std::make_shared<yc::app::threads::renderer>(
        shared_from_this(), renderer_profiler);

  auto logic_profiler = std::make_shared<yc::util::simple_profiler>();
  auto logic_thread   = std::make_shared<yc::app::threads::logic>(
        shared_from_this(), logic_profiler);

  os_events->on_stop_signal.add_listener(
        &renderer_thread->stop_signal_listener);

  logic_thread->start();
  renderer_thread->start();

  renderer_thread->join();
  logging(LOG_INFO, "Renderer thread has finished");

  /* Sync points are no longer needed.
   * Shutdown them to avoid deadlocks */
  common_data.pre_frame_sync.shutdown();
  common_data.post_frame_sync.shutdown();

  /* Ask logic/statistics thread to politely close */
  logic_thread->close();
  logic_thread->join();

  logging(LOG_INFO, "Logic thread has finished");

  return yc::error::OK;
}