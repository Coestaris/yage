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

  virtual void before_loop()                                         = 0;
  virtual void end()                                                 = 0;
  virtual void begin()                                               = 0;
  virtual void on_object(std::shared_ptr<yc::graphics::node> object) = 0;

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