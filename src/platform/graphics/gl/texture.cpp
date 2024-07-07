/** ========================================================================
 *
 * @file texture.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>
#include "texture.hpp"
#include "graphics/texture.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::platform::graphics::gl::texture::load()
{
  if (sources.empty())
  {
    logging(LOG_ERROR, "No sources to load");
    return yage::core::INVALID_ARGUMENT;
  }

  logging(LOG_DEBUG, "Loading texture: %s", name.c_str());

  auto parameters = sources.get_parameters();
  auto uris       = sources.get_uris();

  switch (parameters.type)
  {
    case yage::graphics::tex_type::TEXTURE_2D:
    {
      if (uris.size() != 1)
      {
        logging(
              LOG_ERROR, "Invalid number of images for 2D texture: %" PRIuPTR,
              uris.size());
        return yage::core::INVALID_ARGUMENT;
      }

      std::vector<uint8_t> image_data;
      size_t               image_width, image_height;
      if (!load_image(uris[0], image_data, image_width, image_height))
      {
        logging(
              LOG_ERROR, "Cannot load pixels of image %s", name.c_str());
        return yage::core::INVALID_ARGUMENT;
      }

      GLuint int_format;
      GLuint level = 0;
      switch (parameters.format)
      {
        case yage::graphics::tex_pixel_format::RED:
          int_format = GL_RED;
          break;
        case yage::graphics::tex_pixel_format::RG:
          int_format = GL_RG;
          break;
        case yage::graphics::tex_pixel_format::RGB:
          int_format = GL_RGB;
          break;
        case yage::graphics::tex_pixel_format::RGBA:
          int_format = GL_RGBA;
          break;
        case yage::graphics::tex_pixel_format::DEPTH:
          int_format = GL_DEPTH_COMPONENT;
          break;
        case yage::graphics::tex_pixel_format::STENCIL:
          int_format = GL_STENCIL_INDEX;
          break;
        case yage::graphics::tex_pixel_format::DEPTH_STENCIL:
          int_format = GL_DEPTH_STENCIL;
          break;
        default:
          logging(
                LOG_ERROR, "Unsupported texture format: %s",
                yage::graphics::to_string(parameters.format).c_str());
          return yage::core::INVALID_ARGUMENT;
      }

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glTexImage2D(
            GL_TEXTURE_2D, level, int_format, image_width, image_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());
      ctx->glGenerateMipmap(GL_TEXTURE_2D);

      id     = texture;
      live   = true;
      loaded = true;
    }
    break;

    case yage::graphics::tex_type::TEXTURE_1D:
    case yage::graphics::tex_type::TEXTURE_3D:
    case yage::graphics::tex_type::TEXTURE_CUBE_MAP:
    default:
      logging(
            LOG_ERROR, "Unsupported texture type: %s",
            yage::graphics::to_string(parameters.type).c_str());
      return yage::core::INVALID_ARGUMENT;
  }

  return yage::core::OK;
}

void yage::platform::graphics::gl::texture::unload()
{
  glDeleteTextures(1, &id);
}
