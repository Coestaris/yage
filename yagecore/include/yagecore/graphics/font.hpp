/** ========================================================================
 *
 * @file font.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics.hpp"
#include "glm/vec4.hpp"

#include <string>
#include <memory>

#include "yagecore/app/asset.hpp"
#include "yagecore/graphics/font_param.hpp"
#include "glm/vec2.hpp"

#include <vector>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class font : public yc::app::asset
{
public:
  class data
  {
    yc::util::uri            uri {};
    std::string              work_dir {};
    yc::graphics::font_param parameters {};

  public:
    void set_work_dir(const std::string & work_dir);
    void set_uri(yc::util::uri uri);
    void set_parameters(const yc::graphics::font_param & parameters);

    [[nodiscard]] yc::util::uri            get_uri() const;
    [[nodiscard]] yc::graphics::font_param get_parameters() const;
    [[nodiscard]] std::string              get_work_dir() const;

    bool operator==(const data & other) const;
    bool empty() const;
  };

public:
  struct char_cache
  {
    unsigned int x_advance;
    unsigned int y_advance;

    size_t internal_index;
  };

protected:
  std::vector<char_cache>                 cache;
  std::shared_ptr<yc::graphics::graphics> graphics;
  data                                    sources;

  font(std::string                             name,
       std::shared_ptr<yc::app::asset_cacher>  cacher,
       std::shared_ptr<yc::graphics::graphics> graphics);

  virtual void draw_begin(glm::vec4 color) = 0;
  virtual void draw_end()                  = 0;
  virtual void
  draw(const char_cache & c, glm::vec2 pos)
        = 0;

public:
  void set_sources(const data & _sources);

  void
  draww(const std::string & text,
        glm::vec2           pos,
        glm::vec4           color = { 1.0f, 1.0f, 1.0f, 1.0f });
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}