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

#include "graphics/image.hpp"
#include "graphics/graphics.hpp"

#include "core/error.hpp"
#include "core/io/mouse.hpp"
#include "core/io/keyboard.hpp"
#include "graphics/input_stream.hpp"

namespace yage::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class settings
{
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
public:
  yage::graphics::input_stream input;

  window(settings & settings)
  {
  }
  virtual ~window() = default;

  virtual yage::core::error
  open(std::string & title, geometry & geometry, bool fullscreen)
        = 0;
  virtual yage::core::error          update()       = 0;
  virtual yage::core::error          close()        = 0;
  virtual bool                       is_open()      = 0;
  virtual yage::graphics::graphics * get_graphics() = 0;

  virtual yage::core::error set_fullscreen(bool fullscreen)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }

  virtual yage::core::error set_vsync(bool vsync)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }

  virtual yage::core::error set_icon(image & icon)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }

  virtual yage::core::error set_cursor(image & cursor)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }

  virtual yage::core::error set_geometry(geometry & geometry)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }

  virtual yage::core::error set_title(char * title)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
