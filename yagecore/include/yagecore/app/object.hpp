/** ========================================================================
 *
 * @file game_object.hpp
 * @date 19-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "glm/vec2.hpp"

#include "yagecore/time.hpp"
#include "yagecore/graphics/node.hpp"

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
  class yc::app::application * app;

  virtual void update(yc::milliseconds delta) = 0;

public:
  struct renderer_data
  {
    bool                 visible;
    yc::graphics::node * primitive;

  } data = { false, nullptr };

  class logic_data
  {
  private:
    bool should_be_deleted = false;

    friend class yc::app::threads::logic;
    friend class object;
    friend class application;

  } logic_data;

  virtual ~object() = default;
  explicit object(yc::app::application * app)
  : app(app) {};

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