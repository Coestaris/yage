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
#include "core/error.hpp"
#include "graphics/graphics.hpp"

namespace yage::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class settings
{
};

class image
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
  window(settings & settings)
  {
  }
  virtual yage::core::error
  open(std::string & title, geometry & geometry, bool fullscreen)
        = 0;
  virtual yage::core::error          update()       = 0;
  virtual yage::core::error          close()        = 0;
  virtual bool                       is_open()      = 0;
  virtual yage::graphics::graphics * get_graphics() = 0;

  yage::core::error set_fullscreen(bool fullscreen)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
  yage::core::error set_vsync(bool vsync)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
  yage::core::error set_icon(image & icon)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
  yage::core::error set_cursor(image & cursor)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
  yage::core::error set_geometry(geometry & geometry)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
  yage::core::error set_title(char * title)
  {
    return yage::core::error::NOT_IMPLEMENTED;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */

}
