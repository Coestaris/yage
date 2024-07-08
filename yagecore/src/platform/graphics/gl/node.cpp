/** ========================================================================
 *
 * @file node.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>
#include "yagecore/log.hpp"
#include "yagecore/error.hpp"
#include "yagecore/gl/node.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::error yc::gl::node::load()
{
  live = true;

  logging(LOG_DEBUG, "Loading GLTF node: %s", get_name().c_str());

  if (!load_gltf(sources.get_uri()))
  {
    logging(LOG_ERROR, "Failed to load GLTF node: %s", get_name().c_str());
    return yc::error::EXTERNAL_API_ERROR;
  }

  live   = true;
  loaded = true;

  return yc::error::OK;
}

void yc::gl::node::unload()
{
}
