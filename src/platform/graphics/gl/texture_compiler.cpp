/** ========================================================================
 *
 * @file texture_compiler.cpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
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
    log(LOG_ERROR, "No images to compile");
    return yage::core::INVALID_STATE;
  }

  binary_data.clear();
  switch (parameters.type)
  {
    case yage::graphics::tex_type::TEXTURE_2D:
    {
      if (image_paths.size() != 1)
      {
        log(LOG_ERROR, "Invalid number of images for 2D texture: %lld",
            image_paths.size());
        return yage::core::INVALID_ARGUMENT;
      }

      std::vector<rgba8888> image_data;
      yage::core::float2    image_size;
      if (!image_to_rgba8888(image_paths[0], image_data, image_size))
      {
        log(LOG_ERROR, "Failed to load image: %s", image_paths[0].c_str());
        return yage::core::INVALID_ARGUMENT;
      }

      /* TODO: FOR NOW, */
      binary_data.resize(image_data.size() * 4);
      for (size_t i = 0; i < image_data.size(); ++i)
      {
        binary_data[i * 4 + 0] = image_data[i].r;
        binary_data[i * 4 + 1] = image_data[i].g;
        binary_data[i * 4 + 2] = image_data[i].b;
        binary_data[i * 4 + 3] = image_data[i].a;
      }
    }
    break;

    case yage::graphics::tex_type::TEXTURE_1D:
    case yage::graphics::tex_type::TEXTURE_3D:
    case yage::graphics::tex_type::TEXTURE_CUBE_MAP:
    default:
      log(LOG_ERROR, "Unsupported texture type: %s",
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
