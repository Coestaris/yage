/** ========================================================================
 *
 * @file shader.hpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <vector>
#include <cstdint>

#include "graphics/graphics.hpp"
#include "core/float2.hpp"
#include "core/float3.hpp"
#include "shader_type.hpp"

namespace yage::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class shader_compiler
{
protected:
  yage::graphics::graphics * graphics;

public:

  class compile_message
  {
  public:
    std::string message;
    int         line;
    enum
    {
      TYPE_ERROR,
      TYPE_WARNING,
      TYPE_INFO,
    } type;
  };

  explicit shader_compiler(yage::graphics::graphics * graphics)
  : graphics(graphics)
  {
  }

  virtual ~shader_compiler() = default;

  /** Clears all shader sources for further compilation */
  virtual void clear() = 0;

  /** Adds shader source to the compiler */
  virtual yage::core::error
  add_shader(std::string source, yage::graphics::shader_type type)
        = 0;

  /** Gets hash of shader sources, so we can check if we need to recompile */
  virtual std::string get_hash() = 0;

  /** Compiles shader sources and returns compilation messages.
   * Note, that if there are errors, binary representation of shader is not
   * available. Shader is deleted from video memory after compilation,
   * so you need to download it again. */
  virtual yage::core::error compile(std::vector<compile_message> & out_messages)
        = 0;

  /** Returns binary representation of compiled shader.
   * Available only after successful compilation */
  virtual yage::core::error get_binary(std::vector<uint8_t> & out_data) = 0;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}