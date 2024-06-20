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

namespace yage::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class application;

class object
{
protected:
  class yage::app::application * app;

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

  virtual void update() = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}