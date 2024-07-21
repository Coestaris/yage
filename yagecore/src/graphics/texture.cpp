/** ========================================================================
 *
 * @file texture.cpp
 * @date 26-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/texture.hpp"
#include "yagecore/log.hpp"
#include "FreeImage.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
__attribute__((constructor(101))) static void init()
{
  FreeImage_Initialise();
}

/* Shared functions ========================================================= */
bool yc::graphics::texture::load_image(
      const yc::util::uri &  uri,
      std::vector<uint8_t> & out_data,
      size_t &               out_w,
      size_t &               out_h)
{
  FreeImage_SetOutputMessage([](FREE_IMAGE_FORMAT, const char * message) {
    logging(LOG_ERROR, "FreeImage: %s", message);
  });

  FreeImage_GetFIFFromMime(uri.get_mime().c_str());

  std::vector<uint8_t> image_data;
  uri.read(image_data);
  if (image_data.empty())
  {
    logging(
          LOG_ERROR, "Failed to read image data from URI: %s",
          uri.get_uri().c_str());
    return false;
  }

  FIMEMORY * data = FreeImage_OpenMemory(image_data.data(), image_data.size());
  FIBITMAP * bitmap = FreeImage_LoadFromMemory(
        FreeImage_GetFIFFromMime(uri.get_mime().c_str()), data);
  FreeImage_CloseMemory(data);

  if (!bitmap)
  {
    logging(LOG_ERROR, "Failed to parse image data: %s", uri.get_uri().c_str());
    return false;
  }

  out_w = FreeImage_GetWidth(bitmap);
  out_h = FreeImage_GetHeight(bitmap);

  out_data.resize(out_w * out_h * 4);

  for (unsigned int y = 0; y < out_h; ++y)
  {
    for (unsigned int x = 0; x < out_w; ++x)
    {
      RGBQUAD color;
      FreeImage_GetPixelColor(bitmap, x, y, &color);

      out_data[4 * (y * out_w + x) + 0] = color.rgbRed;
      out_data[4 * (y * out_w + x) + 1] = color.rgbGreen;
      out_data[4 * (y * out_w + x) + 2] = color.rgbBlue;
      out_data[4 * (y * out_w + x) + 3] = color.rgbReserved;
    }
  }

  FreeImage_Unload(bitmap);
  return true;
}

void yc::graphics::texture::data::add_uri(yc::util::uri uri)
{
  uris.push_back(uri);
}

void yc::graphics::texture::data::set_parameters(
      const texture_param & _parameters)
{
  parameters = _parameters;
}

void yc::graphics::texture::data::set_work_dir(const std::string & dir)
{
  work_dir = dir;
}

bool yc::graphics::texture::data::operator==(const data & other) const
{
  return uris == other.uris && parameters == other.parameters;
}

bool yc::graphics::texture::data::empty() const
{
  return uris.empty();
}

std::vector<yc::util::uri> yc::graphics::texture::data::get_uris() const
{
  return uris;
}

std::string yc::graphics::texture::data::get_work_dir() const
{
  return work_dir;
}

yc::graphics::texture_param yc::graphics::texture::data::get_parameters() const
{
  return parameters;
}

yc::graphics::texture::texture(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
: asset(std::move(name), std::move(cacher))
, graphics(std::move(graphics))
{
}

void yc::graphics::texture::set_sources(const data & _sources)
{
  /* TODO: check if sources are different */
  if (sources.empty())
  //    if (!(sources == _sources))
  {
    logging(LOG_INFO, "Texture %s marked to reload", name.c_str());
    sources = _sources;
    live    = false;
  }
}