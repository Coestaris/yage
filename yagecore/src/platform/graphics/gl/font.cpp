/** ========================================================================
 *
 * @file font.cpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/gl/font.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::error yc::gl::font::load()
{
  if (loaded)
  {
    return OK;
  }

  live = true;

  /* TODO: LOAD */

  loaded = true;

  return yc::error::OK;
}

void yc::gl::font::unload()
{
  if (!loaded)
  {
    return;
  }

  /* TODO: UNLOAD */

  loaded = false;
}

void yc::gl::font::draw(
      const yc::graphics::font::char_cache & c,
      glm::vec2                              pos,
      glm::vec4                              color,
      glm::vec2                              scale)
{
}