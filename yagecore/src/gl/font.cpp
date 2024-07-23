/** ========================================================================
 *
 * @file font.cpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/gl/font.hpp"

#if __has_include("freetype2/ft2build.h")
#include "freetype2/ft2build.h"
#else
#include "freetype/include/ft2build.h"
#endif
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
, ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
{
}

void yc::gl::font::set_char_renderer(
      std::shared_ptr<yc::gl::char_renderer> renderer)
{
  this->char_renderer = std::move(renderer);
};

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
    logging(LOG_ERROR, "Cannot load font '%s'", name.c_str());
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
    if (auto ft_err = FT_Load_Char(face, c, FT_LOAD_RENDER))
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

  /* 2 floats pos, 2 floats uv.
   * 4 vertices per quad */
  struct __attribute__((packed)) quad
  {
    float a_pos[2];
    float a_uv[2];
    float b_pos[2];
    float b_uv[2];
    float c_pos[2];
    float c_uv[2];
    float d_pos[2];
    float d_uv[2];
  };
  std::vector<quad>   quads;
  std::vector<GLuint> indices;

  size_t               atlas_offset = 0;
  std::vector<uint8_t> atlas_data(atlas_width * atlas_height, 0);
  for (int c = sources.get_parameters().get_start_char();
       c <= sources.get_parameters().get_end_char(); ++c)
  {
    /* Render glyph */
    FT_Load_Char(face, c, FT_LOAD_RENDER);
    if (auto ft_err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
    {
      logging(LOG_ERROR, "Cannot render glyph: %s", FT_Error_String(ft_err));
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return yc::INVALID_ARGUMENT;
    }

    /* Blit glyph to atlas */
    uint8_t *    dst        = atlas_data.data() + atlas_offset;
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

    const size_t rectangle_indicies[] = {
      0, 1, 2, 0, 2, 3,
    };
    const size_t start_index = indices.size();
    for (size_t i = 0; i < 6; ++i)
    {
      /* *4 because 4 vertices per quad */
      indices.push_back(quads.size() * 4 + rectangle_indicies[i]);
    }

    float w = face->glyph->bitmap.width;
    float h = face->glyph->bitmap.rows;

    float xpos = face->glyph->bitmap_left;
    float ypos = h - face->glyph->bitmap_top;

    /* Add quad */
    quads.push_back({
          .a_pos = { xpos, ypos + h },
          .a_uv  = { 0, 0 },

          .b_pos = { xpos, ypos },
          .b_uv  = { 0, 1 },

          .c_pos = { xpos + w, ypos },
          .c_uv  = { 1, 1 },

          .d_pos = { xpos + w, ypos + h },
          .d_uv  = { 1, 0 },
    });

    size_t cache_index = c - sources.get_parameters().get_start_char();
    cache[cache_index].x_advance      = face->glyph->advance.x >> 6;
    cache[cache_index].y_advance      = face->glyph->advance.y >> 6;
    cache[cache_index].internal_index = start_index;

    atlas_offset += face->glyph->bitmap.width;
  }

  if (atlas_offset != atlas_width)
  {
    logging(
          LOG_ERROR, "Atlas width mismatch: %lu != %lu", atlas_offset,
          atlas_width);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return yc::INVALID_ARGUMENT;
  }

  /* Create atlas texture */
  // atlas = std::make_shared<yc::gl::texture>(name + "_atlas", cacher, ctx);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_RED,
        GL_UNSIGNED_BYTE, atlas_data.data());
  yc::gl::glGenerateMipmap(GL_TEXTURE_2D);

  /* Generate buffers */
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(
        GL_ARRAY_BUFFER, quads.size() * sizeof(quad), quads.data(),
        GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  /* Dump atlas for debugging */
  FILE * atlas_file = fopen("atlas.raw", "wb");
  if (atlas_file)
  {
    uint32_t w = atlas_width, h = atlas_height;
    fwrite(&w, 1, sizeof(w), atlas_file);
    fwrite(&h, 1, sizeof(h), atlas_file);
    fwrite(atlas_data.data(), 1, atlas_data.size(), atlas_file);
    fclose(atlas_file);
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

void yc::gl::font::draw_begin(glm::vec4 color)
{
#if USE_DEBUG
  if (!char_renderer)
  {
    logging(LOG_ERROR, "No char renderer set");
    return;
  }
  if (!loaded)
  {
    return;
  }
#endif
  char_renderer->begin(vao, color, id);
}

void yc::gl::font::draw_end()
{
#if USE_DEBUG
  if (!char_renderer)
  {
    logging(LOG_ERROR, "No char renderer set");
    return;
  }
  if (!loaded)
  {
    return;
  }
#endif
  char_renderer->end();
}

void yc::gl::font::draw(const yc::graphics::font::char_cache & c, glm::vec2 pos)
{
#if USE_DEBUG
  if (!char_renderer)
  {
    logging(LOG_ERROR, "No char renderer set");
    return;
  }
  if (!loaded)
  {
    return;
  }
#endif

  char_renderer->on_char(c, glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0)));
}