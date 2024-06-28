/** ========================================================================
 *
 * @file texture_compiler.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <GL/gl.h>
#include "texture_compiler.hpp"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::platform::graphics::gl::texture_compiler::clear()
{
  binary_data.clear();
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

yage::core::error yage::platform::graphics::gl::texture_compiler::compile()
{
  if (image_paths.empty())
  {
    logging(LOG_ERROR, "No images to compile");
    return yage::core::INVALID_STATE;
  }

  binary_data.clear();
  switch (parameters.type)
  {
    case yage::graphics::tex_type::TEXTURE_2D:
    {
      if (image_paths.size() != 1)
      {
        logging(LOG_ERROR, "Invalid number of images for 2D texture: %zu",
            image_paths.size());
        return yage::core::INVALID_ARGUMENT;
      }

      std::vector<rgba8888> image_data;
      glm::vec2    image_size;
      if (!image_to_rgba8888(image_paths[0], image_data, image_size))
      {
        logging(LOG_ERROR, "Failed to load image: %s", image_paths[0].c_str());
        return yage::core::INVALID_ARGUMENT;
      }

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);

      switch (parameters.format)
      {

        case yage::graphics::tex_pixel_format::RED:
        {
          std::vector<uint8_t> red_data;
          red_data.reserve(image_data.size());
          for (const rgba8888 & pixel : image_data)
          {
            red_data.push_back(pixel.r);
          }

          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED, image_size.x, image_size.y, 0, GL_RED,
                GL_UNSIGNED_BYTE, red_data.data());
        }
        break;
        case yage::graphics::tex_pixel_format::RG:
        {
          std::vector<uint8_t> rg_data;
          rg_data.reserve(image_data.size() * 2);
          for (const rgba8888 & pixel : image_data)
          {
            rg_data.push_back(pixel.r);
            rg_data.push_back(pixel.g);
          }

          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RG, image_size.x, image_size.y, 0, GL_RG,
                GL_UNSIGNED_BYTE, rg_data.data());
        }
        break;
        case yage::graphics::tex_pixel_format::RGB:
        {
          std::vector<uint8_t> rgb_data;
          rgb_data.reserve(image_data.size() * 3);
          for (const rgba8888 & pixel : image_data)
          {
            rgb_data.push_back(pixel.r);
            rgb_data.push_back(pixel.g);
            rgb_data.push_back(pixel.b);
          }

          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGB, image_size.x, image_size.y, 0, GL_RGB,
                GL_UNSIGNED_BYTE, rgb_data.data());
        }
        break;
        case yage::graphics::tex_pixel_format::RGBA:
        {
          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA, image_size.x, image_size.y, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());
        }
        break;
        case yage::graphics::tex_pixel_format::DEPTH:
        {
          std::vector<uint8_t> depth_data;
          depth_data.reserve(image_data.size());
          for (const rgba8888 & pixel : image_data)
          {
            depth_data.push_back(pixel.r);
          }

          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, image_size.x,
                image_size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                depth_data.data());
        }
        break;
        case yage::graphics::tex_pixel_format::STENCIL:
        {
          std::vector<uint8_t> stencil_data;
          stencil_data.reserve(image_data.size());
          for (const rgba8888 & pixel : image_data)
          {
            stencil_data.push_back(pixel.r);
          }

          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, image_size.x, image_size.y,
                0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_data.data());
        }
        break;
        case yage::graphics::tex_pixel_format::DEPTH_STENCIL:
        {
          std::vector<uint8_t> depth_stencil_data;
          depth_stencil_data.reserve(image_data.size() * 2);
          for (const rgba8888 & pixel : image_data)
          {
            depth_stencil_data.push_back(pixel.r);
            depth_stencil_data.push_back(pixel.g);
          }

          glTexImage2D(
                GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, image_size.x, image_size.y,
                0, GL_DEPTH_STENCIL, GL_UNSIGNED_BYTE,
                depth_stencil_data.data());
        }
        break;

        default:
          logging(LOG_ERROR, "Unsupported texture format: %s",
              yage::graphics::to_string(parameters.format).c_str());
          return yage::core::INVALID_ARGUMENT;
      }


    }
    break;

    case yage::graphics::tex_type::TEXTURE_1D:
    case yage::graphics::tex_type::TEXTURE_3D:
    case yage::graphics::tex_type::TEXTURE_CUBE_MAP:
    default:
      logging(LOG_ERROR, "Unsupported texture type: %s",
          yage::graphics::to_string(parameters.type).c_str());
      return yage::core::INVALID_ARGUMENT;
  }

  return yage::core::OK;
}

yage::core::error yage::platform::graphics::gl::texture_compiler::get_binary(
      std::vector<uint8_t> & out_data)
{
  out_data = binary_data;
  return yage::core::OK;
}
