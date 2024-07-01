/** ========================================================================
 *
 * @file texture_compiler.cpp
 * @date 26-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "texture_compiler.hpp"
#include "FreeImage.h"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yage::graphics::texture_compiler::load_image(
      const std::string &      path,
      std::vector<glm::vec4> & out_data,
      size_t &                 out_w,
      size_t &                 out_h)
{
  FreeImage_SetOutputMessage([](FREE_IMAGE_FORMAT, const char * message) {
    logging(LOG_ERROR, "FreeImage: %s", message);
  });
  FIBITMAP * bitmap = FreeImage_Load(
        FreeImage_GetFIFFromFilename(path.c_str()), path.c_str());
  if (!bitmap)
  {
    return false;
  }

  out_w = FreeImage_GetWidth(bitmap);
  out_h = FreeImage_GetHeight(bitmap);

  out_data.resize(out_w * out_h);

  for (unsigned int y = 0; y < out_h; ++y)
  {
    for (unsigned int x = 0; x < out_w; ++x)
    {
      RGBQUAD color;
      FreeImage_GetPixelColor(bitmap, x, y, &color);

      out_data[y * out_w + x].r = color.rgbRed;
      out_data[y * out_w + x].g = color.rgbGreen;
      out_data[y * out_w + x].b = color.rgbBlue;
      out_data[y * out_w + x].a = color.rgbReserved;
    }
  }

  FreeImage_Unload(bitmap);
  return true;
}
