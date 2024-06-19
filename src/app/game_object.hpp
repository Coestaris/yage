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
class game_object
{
public:
  virtual ~game_object() {};

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