/** ========================================================================
 *
 * @file game_object.hpp
 * @date 19-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "glm/vec2.hpp"

#include "yagecore/time.hpp"
#include "yagecore/graphics/node.hpp"
#include "yagecore/app/asset_factory.hpp"

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class application;

namespace threads
{
class logic;
}

class object
{
private:
  yc::milliseconds last_update = 0;

protected:
  std::shared_ptr<yc::app::application> app;

public:
  struct renderer_data
  {
    bool                                visible;
    bool                                ready;
    glm::mat4                           transform;
    std::shared_ptr<yc::graphics::node> node;

  } render = { false, false, glm::mat4(1.0f), nullptr };

  class logic_data
  {
  private:
    bool should_be_deleted = false;

    friend class yc::app::threads::logic;
    friend class object;
    friend class application;

  } logic_data;

  virtual ~object() = default;
  explicit object(std::shared_ptr<yc::app::application> app)
  : app(std::move(app)) {};

protected:
  virtual void update(yc::milliseconds delta) = 0;

public:
  virtual void on_ready(const std::shared_ptr<asset_factory> & factory) = 0;

  void update()
  {
    yc::milliseconds delta;
    yc::milliseconds time = yc::get_ms();
    if (last_update == 0)
    {
      last_update = time;
      delta       = 0;
    }
    else
    {
      delta       = time - last_update;
      last_update = time;
    }

    update(delta);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}