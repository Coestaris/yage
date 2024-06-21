/** ========================================================================
 *
 * @file game_object.hpp
 * @date 19-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "core/float2.hpp"
#include "core/time.hpp"

namespace yage::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class application;

class object
{
private:
  yage::core::milliseconds last_update = 0;

protected:
  class yage::app::application * app;

  virtual void update(yage::core::milliseconds delta) = 0;

public:
  virtual ~object() = default;
  explicit object(yage::app::application * app)
  : app(app) {};

  /* for now, let's assume that all game objects have position */
  struct public_data
  {
    yage::core::float2 position;
    yage::core::float2 size;
  } data;

  void update()
  {
    yage::core::milliseconds delta;
    yage::core::milliseconds time = yage::core::get_ms();
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