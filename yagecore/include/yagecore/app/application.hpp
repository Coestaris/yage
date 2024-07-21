/** ========================================================================
 *
 * @file threads.hpp
 * @date 18-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <algorithm>
#include <utility>

#include "yagecore/os_events.hpp"
#include "yagecore/util/sync_point.hpp"
#include "yagecore/util/profiler.hpp"
#include "yagecore/graphics/window.hpp"
#include "yagecore/app/object.hpp"
#include "yagecore/app/input_manager.hpp"
#include "yagecore/app/statistics.hpp"
#include "yagecore/app/asset_factory.hpp"

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
namespace threads
{
class logic;
class renderer;
}

class application : public std::enable_shared_from_this<application>
{
  std::shared_ptr<yc::os_events>        os_events;
  std::shared_ptr<yc::graphics::window> window;
  std::shared_ptr<input_manager>        input_mgr;

public:
  class common_data
  {
    std::vector<std::shared_ptr<yc::app::object>> objects;
    yc::util::sync_point pre_frame_sync { 2 };
    yc::util::sync_point post_frame_sync { 2 };

    friend class yc::app::threads::logic;
    friend class yc::app::threads::renderer;
    friend class application;

  } common_data;

  class logic_data
  {
    std::vector<std::shared_ptr<yc::app::object>> object_to_add;
    std::mutex                                    object_to_add_mutex;

    friend class yc::app::threads::logic;
    friend class application;

  } logic_data;

  /* acts like renderer data,
   * and additionally used to perform custom rendering */
  class render_data
  {
    std::shared_ptr<yc::app::asset_factory>  asset_factory;
    std::shared_ptr<yc::graphics::window>    window;
    std::vector<yc::app::object::renderable> objects;

    friend class yc::app::threads::renderer;
    friend class application;

  public:
    render_data(
          std::shared_ptr<yc::app::asset_factory> asset_factory,
          std::shared_ptr<yc::graphics::window>   window);

  } render_data;

  yc::app::statistics stats;

  virtual ~application() = default;

  application(
        std::shared_ptr<yc::app::asset_factory>       asset_factory,
        const std::shared_ptr<yc::graphics::window> & window,
        std::shared_ptr<yc::os_events>                os_events);

  virtual std::vector<std::shared_ptr<yc::graphics::render_pass>>
  create_render_chain(
        const std::shared_ptr<yc::graphics::graphics> & graphics,
        const std::shared_ptr<yc::app::asset_factory> & asset_factory)
        = 0;

  yc::error start();
  void      stop();

  void add_object(std::shared_ptr<yc::app::object> object);
  void destroy_object(
        const std::shared_ptr<yc::app::object> & object, bool hard = false);
  void destroy_all_objects(bool hard = false);

  [[nodiscard]] yc::graphics::geometry get_geometry() const;
  std::shared_ptr<input_manager>       get_input();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}