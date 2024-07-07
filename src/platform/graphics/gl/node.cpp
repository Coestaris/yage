/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>
#include "node.hpp"
#include "graphics/node.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::platform::graphics::gl::node::load()
{
  logging(LOG_DEBUG, "Loading GLTF node: %s", get_name().c_str());

  if (!load_gltf(sources.get_uri()))
  {
    logging(LOG_ERROR, "Failed to load GLTF node: %s", get_name().c_str());
    return yage::core::error::EXTERNAL_API_ERROR;
  }

  live   = true;
  loaded = true;

  return yage::core::error::OK;
}

void yage::platform::graphics::gl::node::unload()
{
}
