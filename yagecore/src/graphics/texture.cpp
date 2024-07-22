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

#include "libpng/png.h"

#include <cstring>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
bool yc::graphics::texture::load_image(
      const yc::util::uri &  uri,
      std::vector<uint8_t> & out_data,
      size_t &               out_w,
      size_t &               out_h)
{
  std::vector<uint8_t> image_data;
  uri.read(image_data);
  if (image_data.empty())
  {
    logging(
          LOG_ERROR, "Failed to read image data from URI: %s",
          uri.get_uri().c_str());
    return false;
  }

  png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr)
  {
    logging(LOG_ERROR, "Failed to create PNG read struct");
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    logging(LOG_ERROR, "Failed to create PNG info struct");
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return false;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    logging(LOG_ERROR, "Failed to set PNG jump buffer");
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return false;
  }

  /* Set up the input control */
  struct ReadPngData
  {
    const std::vector<uint8_t> * data;
    size_t                       offset;
  };

  auto read_png_data = [](png_structp png_ptr, png_bytep outBytes,
                          png_size_t byteCountToRead) {
    auto * readPngData = static_cast<ReadPngData *>(png_get_io_ptr(png_ptr));
    if (readPngData->offset + byteCountToRead > readPngData->data->size())
    {
      png_error(png_ptr, "Read error");
    }
    memcpy(
          outBytes, readPngData->data->data() + readPngData->offset,
          byteCountToRead);
    readPngData->offset += byteCountToRead;
  };
  ReadPngData readPngData = { &image_data, 0 };
  png_set_read_fn(png_ptr, &readPngData, read_png_data);

  // Read the PNG info
  png_read_info(png_ptr, info_ptr);

  // Get image information
  png_uint_32 width      = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height     = png_get_image_height(png_ptr, info_ptr);
  png_byte    color_type = png_get_color_type(png_ptr, info_ptr);
  png_byte    bit_depth  = png_get_bit_depth(png_ptr, info_ptr);

  out_w = width;
  out_h = height;

  // Ensure the image is in RGBA format
  if (color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_set_palette_to_rgb(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
  {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
  {
    png_set_tRNS_to_alpha(png_ptr);
  }
  if (bit_depth == 16)
  {
    png_set_strip_16(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY
      || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    png_set_gray_to_rgb(png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY
      || color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
  }

  png_read_update_info(png_ptr, info_ptr);

  // Allocate memory for the image data
  out_data.resize(width * height * 4); // 4 bytes per pixel for RGBA
  png_bytep * row_pointers = new png_bytep[height];
  for (png_uint_32 y = 0; y < height; y++)
  {
    row_pointers[y] = out_data.data() + y * width * 4;
  }

  // Read the image data
  png_read_image(png_ptr, row_pointers);

  // Cleanup
  delete[] row_pointers;
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

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