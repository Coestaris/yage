/** ========================================================================
 *
 * @file texture.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>
#include "yagecore/gl/texture.hpp"
#include "yagecore/graphics/texture.hpp"
#include "yagecore/graphics/graphics.hpp"
#include "yagecore/error.hpp"
#include "yagecore/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::error yc::gl::texture::load()
{
  if (loaded)
  {
    return OK;
  }

  live = true;

  if (sources.empty())
  {
    logging(LOG_ERROR, "No sources to load");
    return yc::INVALID_ARGUMENT;
  }

  logging(LOG_DEBUG, "Loading texture: %s", name.c_str());

  auto parameters = sources.get_parameters();
  auto uris       = sources.get_uris();

  switch (parameters.get_type())
  {
    case yc::graphics::tex_type::TEXTURE_2D:
    {
      if (uris.size() != 1)
      {
        logging(
              LOG_ERROR, "Invalid number of images for 2D texture: %" PRIuPTR,
              uris.size());
        return yc::INVALID_ARGUMENT;
      }

      std::vector<std::byte> image_data;
      size_t                 image_width, image_height;
      if (!load_image(uris[0], image_data, image_width, image_height))
      {
        logging(LOG_ERROR, "Cannot load pixels of image %s", name.c_str());
        return yc::INVALID_ARGUMENT;
      }

      GLuint int_format;
      switch (parameters.get_format())
      {
        case yc::graphics::tex_pixel_format::RED:
          int_format = GL_RED;
          break;
        case yc::graphics::tex_pixel_format::RG:
          int_format = GL_RG;
          break;
        case yc::graphics::tex_pixel_format::RGB:
          int_format = GL_RGB;
          break;
        case yc::graphics::tex_pixel_format::RGBA:
          int_format = GL_RGBA;
          break;
        case yc::graphics::tex_pixel_format::DEPTH:
          int_format = GL_DEPTH_COMPONENT;
          break;
        case yc::graphics::tex_pixel_format::STENCIL:
          int_format = GL_STENCIL_INDEX;
          break;
        case yc::graphics::tex_pixel_format::DEPTH_STENCIL:
          int_format = GL_DEPTH_STENCIL;
          break;
        default:
          logging(
                LOG_ERROR, "Unsupported texture format: %s",
                yc::graphics::to_string(parameters.get_format()).c_str());
          return yc::INVALID_ARGUMENT;
      }

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glTexImage2D(
            GL_TEXTURE_2D, 0, int_format, image_width, image_height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, image_data.data());
      yc::gl::glGenerateMipmap(GL_TEXTURE_2D);

      id     = texture;
      live   = true;
      loaded = true;
    }
    break;

    case yc::graphics::tex_type::TEXTURE_1D:
    case yc::graphics::tex_type::TEXTURE_3D:
    case yc::graphics::tex_type::TEXTURE_CUBE_MAP:
    default:
      logging(
            LOG_ERROR, "Unsupported texture type: %s",
            yc::graphics::to_string(parameters.get_type()).c_str());
      return yc::INVALID_ARGUMENT;
  }

  return yc::OK;
}

void yc::gl::texture::unload()
{
  if (!loaded)
  {
    return;
  }

  glDeleteTextures(1, &id);

  loaded = false;
}

yc::gl::texture::texture(
      std::string                                     name,
      std::shared_ptr<yc::app::asset_cacher>          cacher,
      const std::shared_ptr<yc::graphics::graphics> & graphics)
: yc::graphics::texture(std::move(name), std::move(cacher), graphics)
, ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics)) {};

inline void yc::gl::texture::use(GLenum texture_unit)
{
  yc::gl::glActiveTexture(texture_unit);
  yc::gl::glBindTexture(GL_TEXTURE_2D, id);
}