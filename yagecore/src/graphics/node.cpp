/** ========================================================================
 *
 * @file node.cpp
 * @date 26-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/node.hpp"
#include "yagecore/log.hpp"

/* Suspend -Werror=redundant-move */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-move"
#include "fastgltf/core.hpp"
#pragma GCC diagnostic pop

#include "yagecore/fs.hpp"
#include "yagecore/util/uri.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yc::graphics::node::load_gltf(const yc::util::uri & uri)
{
  fastgltf::Parser parser;
  auto             data    = fastgltf::MappedGltfFile::FromPath(uri.get_uri());
  bool             success = true;
  if (auto error = data.error(); error != fastgltf::Error::None)
  {
    logging(
          LOG_ERROR, "Failed to read GLTF data from URI: %s",
          uri.get_uri().c_str());
    success = false;
  }
  if (!success)
  {
    return false;
  }

  success          = true;
  auto asset_error = parser.loadGltf(
        data.get(), yc::basedir(uri.get_uri()),
        fastgltf::Options::None);
  if (auto error = asset_error.error(); error != fastgltf::Error::None)
  {
    logging(
          LOG_ERROR, "Failed to parse GLTF data from URI: %s",
          uri.get_uri().c_str());
    success = false;
  }
  if (!success)
  {
    return false;
  }

  return true;
}
