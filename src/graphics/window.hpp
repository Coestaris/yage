/** ========================================================================
 *
 * @file window.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <queue>
#include <mutex>
#include <map>

#include "graphics/image.hpp"
#include "graphics/graphics.hpp"

#include "core/error.hpp"
#include "graphics/input_events.hpp"

namespace yage::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
struct settings
{
  std::string title;
  int         width;
  int         height;
  int         max_fps;
  enum class window_mode
  {
    FULLSCREEN,
    WINDOWED_RESIZABLE,
    WINDOWED_FIXED,
  } win_mode;
  bool vsync;

  std::map<std::string, std::string> custom_settings;
};

struct geometry
{
  int x;
  int y;
  int width;
  int height;
};

class window
{
protected:
  yage::graphics::settings settings;

public:
  yage::graphics::input_events             input;

  window(yage::graphics::settings & settings)
  : settings(settings)
  {
  }
  virtual ~window() = default;

  virtual yage::core::error          open()         = 0;
  virtual yage::core::error          update()       = 0;
  virtual yage::core::error          close()        = 0;
  virtual bool                       is_open()      = 0;
  virtual yage::graphics::graphics * get_graphics() = 0;

  virtual yage::core::error set_mode(settings::window_mode mode) = 0;
  virtual yage::core::error get_mode(settings::window_mode & mode)
  {
    mode = settings.win_mode;
    return yage::core::error::OK;
  }

  virtual yage::core::error set_max_fps(int max_fps) = 0;
  virtual yage::core::error get_max_fps(int & max_fps)
  {
    max_fps = settings.max_fps;
    return yage::core::error::OK;
  }

  virtual yage::core::error set_vsync(bool vsync) = 0;
  virtual yage::core::error get_vsync(bool & vsync)
  {
    vsync = settings.vsync;
    return yage::core::error::OK;
  }

  virtual yage::core::error set_icon(image & icon) = 0;
  virtual yage::core::error get_icon(image & icon) = 0;

  virtual yage::core::error set_cursor(image & cursor) = 0;
  virtual yage::core::error get_cursor(image & cursor) = 0;

  virtual yage::core::error set_geometry(geometry & geometry) = 0;
  virtual yage::core::error get_geometry(geometry & geometry)
  {
    geometry = { 0, 0, settings.width, settings.height };
    return yage::core::error::OK;
  }

  virtual yage::core::error set_title(std::string title) = 0;
  virtual yage::core::error get_title(std::string & title)
  {
    title = settings.title;
    return yage::core::error::OK;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
