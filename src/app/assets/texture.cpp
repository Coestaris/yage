/** ========================================================================
 *
 * @file image.cpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>

#include "graphics/texture_compiler.hpp"
#include "app/assets/texture.hpp"
#include "core/hash.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::app::assets::texture::load()
{
  if (is_loaded())
  {
    return yage::core::error::OK;
  }
  live = true;

  yage::graphics::texture_compiler * compiler
        = graphics->create_texture_compiler();

  compiler->set_parameters(parameters);
  for (const std::string & path : image_paths)
  {
    compiler->add_image(path);
  }

  logging(LOG_DEBUG, "Compiling texture: %s", name.c_str());
  if (compiler->compile(pixmap) != yage::core::OK)
  {
    logging(LOG_ERROR, "Failed to compile texture: %s", name.c_str());
    return yage::core::INVALID_STATE;
  }

  graphics->destroy_texture_compiler(compiler);

  loaded = true;

  return yage::core::OK;
}

void yage::app::assets::texture::unload()
{
  if (!is_loaded())
  {
    return;
  }

  if (pixmap)
  {
    pixmap->destroy();
    graphics->destroy_texture(pixmap);
    pixmap = nullptr;
  }

  live   = false;
  loaded = false;
}

std::string yage::app::assets::texture::get_cache_key()
{
  yage::core::fast_hasher hasher;
  hasher.add(reinterpret_cast<uint8_t *>(&parameters), sizeof(parameters));

  /* todo: add image paths to hash */

  return "texture" + name + hasher.as_string();
}
