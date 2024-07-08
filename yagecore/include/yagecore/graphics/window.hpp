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
#include <functional>

#include "yagecore/error.hpp"
#include "yagecore/graphics/input_events.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;
class texture;

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
  yc::graphics::settings settings;
  std::function<void()>  before_close_callback;

public:
  yc::graphics::input_events input;

  window(yc::graphics::settings & settings)
  : settings(settings)
  {
  }
  virtual ~window() = default;

  virtual yc::error                open()         = 0;
  virtual yc::error                update()       = 0;
  virtual yc::error                close()        = 0;
  virtual bool                     is_open()      = 0;
  virtual std::shared_ptr<yc::graphics::graphics> get_graphics() = 0;

  virtual yc::error set_mode(settings::window_mode mode) = 0;
  virtual yc::error get_mode(settings::window_mode & mode)
  {
    mode = settings.win_mode;
    return yc::error::OK;
  }

  virtual yc::error set_max_fps(int max_fps) = 0;
  virtual yc::error get_max_fps(int & max_fps)
  {
    max_fps = settings.max_fps;
    return yc::error::OK;
  }

  virtual yc::error set_vsync(bool vsync) = 0;
  virtual yc::error get_vsync(bool & vsync)
  {
    vsync = settings.vsync;
    return yc::error::OK;
  }

  virtual yc::error set_icon(texture & icon) = 0;
  virtual yc::error get_icon(texture & icon) = 0;

  virtual yc::error set_cursor(texture & cursor) = 0;
  virtual yc::error get_cursor(texture & cursor) = 0;

  virtual yc::error set_geometry(geometry & geometry) = 0;
  virtual yc::error get_geometry(geometry & geometry)
  {
    geometry = { 0, 0, settings.width, settings.height };
    return yc::error::OK;
  }

  virtual yc::error set_title(std::string title) = 0;
  virtual yc::error get_title(std::string & title)
  {
    title = settings.title;
    return yc::error::OK;
  }

  virtual void set_before_close(std::function<void()> callback)
  {
    before_close_callback = callback;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
std::string to_string(yc::graphics::settings::window_mode mode);

}
