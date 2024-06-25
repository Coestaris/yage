/** ========================================================================
 *
 * @file shader_compiler.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>
#include <map>

#include <GL/gl.h>

#include "graphics/shader_compiler.hpp"

namespace yage::platform::graphics::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */

// clang-format off
typedef GLuint (APIENTRY * glCreateShader_proc) (GLenum type);
typedef void (APIENTRY * glShaderSource_proc) (GLuint shader, GLsizei count, const char * const * string, const GLint * length);
typedef void (APIENTRY * glCompileShader_proc) (GLuint shader);
typedef void (APIENTRY * glGetShaderiv_proc) (GLuint shader, GLenum pname, GLint * params);
typedef void (APIENTRY * glGetShaderInfoLog_proc) (GLuint shader, GLsizei bufSize, GLsizei * length, char * infoLog);
typedef void (APIENTRY * glDeleteShader_proc) (GLuint shader);
typedef GLuint (APIENTRY * glCreateProgram_proc) (void);
typedef void (APIENTRY * glAttachShader_proc) (GLuint program, GLuint shader);
typedef void (APIENTRY * glLinkProgram_proc) (GLuint program);
typedef void (APIENTRY * glGetProgramiv_proc) (GLuint program, GLenum pname, GLint * params);
typedef void (APIENTRY * glGetProgramInfoLog_proc) (GLuint program, GLsizei bufSize, GLsizei * length, char * infoLog);
typedef void (APIENTRY * glUseProgram_proc) (GLuint program);
typedef void (APIENTRY * glDeleteProgram_proc) (GLuint program);
typedef void (APIENTRY * glGetProgramBinary_proc) (GLuint program, GLsizei bufSize, GLsizei * length, GLenum * binaryFormat, void * binary);
// clang-format on

class shader_compiler : public yage::graphics::shader_compiler
{
private:
  /* function cache. Because we don't know what
   * version of OpenGL we are using */
  glCreateShader_proc      glCreateShader{};
  glShaderSource_proc      glShaderSource{};
  glCompileShader_proc     glCompileShader{};
  glGetShaderiv_proc       glGetShaderiv{};
  glGetShaderInfoLog_proc  glGetShaderInfoLog{};
  glDeleteShader_proc      glDeleteShader{};
  glCreateProgram_proc     glCreateProgram{};
  glAttachShader_proc      glAttachShader{};
  glLinkProgram_proc       glLinkProgram{};
  glGetProgramiv_proc      glGetProgramiv{};
  glGetProgramInfoLog_proc glGetProgramInfoLog{};
  glUseProgram_proc        glUseProgram{};
  glDeleteProgram_proc     glDeleteProgram{};
  glGetProgramBinary_proc  glGetProgramBinary{};

  /* ordered because we don't want to have
   * different hashes for the same shaders */
  std::map<yage::graphics::shader_type, std::string> sources;

  std::vector<uint8_t> binary;

public:
  explicit shader_compiler(yage::graphics::graphics * graphics);
  ~shader_compiler() override = default;

  void clear() override;
  yage::core::error add_shader(std::string source, yage::graphics::shader_type type) override;
  yage::core::error compile(std::vector<compile_message> & out_messages) override;
  yage::core::error get_binary(std::vector<uint8_t> & out_data) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
