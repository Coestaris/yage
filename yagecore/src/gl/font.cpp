/** ========================================================================
 *
 * @file font.cpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/gl/font.hpp"

#include "freetype2/ft2build.h"
#include FT_FREETYPE_H

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::gl::font::font(
      std::string                                     name,
      std::shared_ptr<yc::app::asset_cacher>          cacher,
      const std::shared_ptr<yc::graphics::graphics> & graphics)
: yc::graphics::font(std::move(name), std::move(cacher), graphics)
, ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics)) {};

yc::error yc::gl::font::load()
{
  if (loaded)
  {
    return OK;
  }

  live = true;

  if (sources.get_uri().empty())
  {
    logging(LOG_ERROR, "No sources to load");
    return yc::INVALID_ARGUMENT;
  }

  cache.resize(
        sources.get_parameters().get_end_char()
        - sources.get_parameters().get_start_char() + 1);

  /* Load font */
  std::vector<uint8_t> ttf_binary;
  sources.get_uri().read(ttf_binary);
  if (ttf_binary.empty())
  {
    logging(LOG_ERROR, "Cannot load font %s", name.c_str());
    return yc::INVALID_ARGUMENT;
  }

  FT_Library library;
  FT_Face    face;
  if (FT_Init_FreeType(&library))
  {
    logging(LOG_ERROR, "Cannot initialize FreeType library");
    return yc::INVALID_ARGUMENT;
  }

  if (auto ft_err = FT_New_Memory_Face(
            library, ttf_binary.data(), ttf_binary.size(), 0, &face))
  {
    logging(LOG_ERROR, "Cannot load font face: %s", FT_Error_String(ft_err));
    FT_Done_FreeType(library);
    return yc::INVALID_ARGUMENT;
  }

  if (auto ft_err
      = FT_Set_Pixel_Sizes(face, 0, sources.get_parameters().get_size()))
  {
    logging(LOG_ERROR, "Cannot set font size: %s", FT_Error_String(ft_err));
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return yc::INVALID_ARGUMENT;
  }

  size_t atlas_width = 0, atlas_height = 0;

  for (int c = sources.get_parameters().get_start_char();
       c <= sources.get_parameters().get_end_char(); ++c)
  {
    if (auto ft_err
        = FT_Load_Char(face, FT_Get_Char_Index(face, c), FT_LOAD_RENDER))
    {
      logging(LOG_ERROR, "Cannot load glyph: %s", FT_Error_String(ft_err));
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return yc::INVALID_ARGUMENT;
    }

    atlas_width += face->glyph->bitmap.width;
    atlas_height = std::max(
          atlas_height, static_cast<size_t>(face->glyph->bitmap.rows));
  }

  size_t               x_offset = 0;
  std::vector<uint8_t> atlas_data(atlas_width * atlas_height, 0);
  for (int c = sources.get_parameters().get_start_char();
       c <= sources.get_parameters().get_end_char(); ++c)
  {
    /* Render glyph */
    FT_Load_Char(face, FT_Get_Char_Index(face, c), FT_LOAD_RENDER);
    if (auto ft_err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
    {
      logging(LOG_ERROR, "Cannot render glyph: %s", FT_Error_String(ft_err));
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return yc::INVALID_ARGUMENT;
    }

    /* Blit glyph to atlas */
    uint8_t *    dst        = atlas_data.data() + x_offset;
    uint8_t *    src        = face->glyph->bitmap.buffer;
    const size_t dst_stride = atlas_width;
    const size_t src_stride = face->glyph->bitmap.width;
    size_t       y          = face->glyph->bitmap.rows;
    while (y--)
    {
      /* Blit row */
      memcpy(dst, src, src_stride);
      dst += dst_stride;
      src += src_stride;
    }

    x_offset += face->glyph->bitmap.width;
  }

  if (x_offset != atlas_width)
  {
    logging(LOG_ERROR, "Atlas width mismatch");
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return yc::INVALID_ARGUMENT;
  }

  // atlas  = std::make_shared<yc::gl::texture>(name + "_atlas", cacher,
  // graphics);
  loaded = true;

  FT_Done_FreeType(library);
  return yc::error::OK;
}

void yc::gl::font::unload()
{
  if (!loaded)
  {
    return;
  }

  if (atlas)
  {
    atlas->unload();
  }

  cache.clear();

  loaded = false;
}

void yc::gl::font::draw(
      const yc::graphics::font::char_cache & c,
      glm::vec2                              pos,
      glm::vec4                              color,
      glm::vec2                              scale)
{
}