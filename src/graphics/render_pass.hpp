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
#include "graphics/node.hpp"

namespace yage::graphics
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
  pass_type type;
  bool skip = false;

public:
  render_pass(std::string name, pass_type type) : name(name), type(type) {}

  virtual ~render_pass() = default;

  virtual void before_loop() = 0;
  virtual void end()   = 0;
  virtual void begin() = 0;
  virtual void on_object(yage::graphics::node * object) = 0;

  virtual void set_name(std::string _name) { this->name = _name; }
  virtual std::string get_name() { return this->name; }
  virtual void set_skip(bool _skip) { this->skip = _skip; }
  virtual bool get_skip() { return this->skip; }
  virtual pass_type get_type() { return this->type; }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}