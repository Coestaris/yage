/** ========================================================================
 *
 * @file texture_compiler.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <cinttypes>
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdexcept>
#include "texture_compiler.hpp"
#include "core/log.hpp"
#include "texture.hpp"
#include "graphics.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::platform::graphics::gl::texture_compiler::clear()
{
  image_paths.clear();
  parameters = yage::graphics::texture_param();
}

yage::core::error
yage::platform::graphics::gl::texture_compiler::set_parameters(
      const yage::graphics::texture_param & param)
{
  parameters = param;
  return yage::core::OK;
}

yage::core::error yage::platform::graphics::gl::texture_compiler::add_image(
      const std::string & path)
{
  image_paths.push_back(path);
  return yage::core::OK;
}

yage::core::error yage::platform::graphics::gl::texture_compiler::compile(
      yage::graphics::texture *& out_texture)
{
  auto * ctx = dynamic_cast<yage::platform::graphics::gl::graphics *>(graphics);

  if (image_paths.empty())
  {
    logging(LOG_ERROR, "No images to compile");
    return yage::core::INVALID_STATE;
  }

  switch (parameters.type)
  {
    case yage::graphics::tex_type::TEXTURE_2D:
    {
      if (image_paths.size() != 1)
      {
        logging(
              LOG_ERROR, "Invalid number of images for 2D texture: %" PRIuPTR,
              image_paths.size());
        return yage::core::INVALID_ARGUMENT;
      }

      std::vector<uint8_t> image_data;
      size_t                 image_width, image_height;
      if (!load_image(image_paths[0], image_data, image_width, image_height))
      {
        logging(LOG_ERROR, "Failed to load image: %s", image_paths[0].c_str());
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

      out_texture = new yage::platform::graphics::gl::texture(
            graphics, parameters, texture);
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

yage::core::error yage::platform::graphics::gl::texture_compiler::add_latent(
      size_t w, size_t h, const std::vector<glm::vec4> & initial_values)
{
  throw std::runtime_error("Not implemented");
  return yage::core::NOT_IMPLEMENTED;
}

yage::core::error yage::platform::graphics::gl::texture_compiler::add_latent(
      size_t w, size_t h, glm::vec4 initial_value)
{
  throw std::runtime_error("Not implemented");
  return yage::core::NOT_IMPLEMENTED;
}