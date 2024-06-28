/** ========================================================================
 *
 * @file texture_compiler.cpp
 * @date 26-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "texture_compiler.hpp"
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "core/log.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static __attribute__((constructor(101))) void init()
{
  FreeImage_Initialise();
}

/* Shared functions ========================================================= */
bool yage::graphics::texture_compiler::image_to_rgba8888(
      const std::string &                                       path,
      std::vector<yage::graphics::texture_compiler::rgba8888> & out_data,
      glm::vec2 &                                      out_size)
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

  out_size.x = FreeImage_GetWidth(bitmap);
  out_size.y = FreeImage_GetHeight(bitmap);

  out_data.resize(out_size.x * out_size.y);

  for (unsigned int y = 0; y < out_size.y; ++y)
  {
    for (unsigned int x = 0; x < out_size.x; ++x)
    {
      RGBQUAD color;
      FreeImage_GetPixelColor(bitmap, x, y, &color);

      out_data[y * out_size.x + x].r = color.rgbRed;
      out_data[y * out_size.x + x].g = color.rgbGreen;
      out_data[y * out_size.x + x].b = color.rgbBlue;
      out_data[y * out_size.x + x].a = color.rgbReserved;
    }
  }

  FreeImage_Unload(bitmap);
  return true;
}
