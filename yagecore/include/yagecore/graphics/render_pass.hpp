/** ========================================================================
 *
 * @file render_pass.hpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <utility>
#include "yagecore/graphics/node.hpp"

#include <yagecore/app/object.hpp>

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum class pass_type
{
  OBJECT_PASS,
  VIRTUAL_PASS,
};

class render_pass
{
private:
  std::string name;
  pass_type   type;
  bool        skip = false;

public:
  render_pass(std::string name, pass_type type)
  : name(std::move(name))
  , type(type)
  {
  }

  virtual ~render_pass() = default;

  /** Called once before rendering loop */
  virtual void before_loop() = 0;

  /** Called before rendering all objects */
  virtual void end() = 0;
  /** Called after rendering all objects */
  virtual void begin() = 0;
  /** Called before rendering single object */
  virtual void on_node(const yc::app::object::renderer_data & object) = 0;
  /** Called before rendering single mesh */
  virtual void on_mesh(std::shared_ptr<yc::graphics::material> material)
        = 0;

  virtual void set_name(std::string _name)
  {
    this->name = std::move(_name);
  }
  virtual std::string get_name()
  {
    return this->name;
  }
  virtual void set_skip(bool _skip)
  {
    this->skip = _skip;
  }
  virtual bool get_skip()
  {
    return this->skip;
  }
  virtual pass_type get_type()
  {
    return this->type;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}