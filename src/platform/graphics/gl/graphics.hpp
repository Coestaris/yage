/** ========================================================================
 *
 * @file graphics.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <vector>
#include <string>

#include "GL/gl.h"
#include "GL/glext.h"
#include "graphics/graphics.hpp"
#include "graphics/texture_compiler.hpp"

namespace yage::platform::graphics::gl
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics : public yage::graphics::graphics
{
private:
  std::vector<std::string> extensions;

  void cache_extensions();
  void print_extensions();
  bool has_extension(const char * extension);

public:
  /* function cache. Because we don't know what
   * version of OpenGL we are using */
  PFNGLCREATESHADERPROC            glCreateShader {};
  PFNGLSHADERSOURCEPROC            glShaderSource {};
  PFNGLCOMPILESHADERPROC           glCompileShader {};
  PFNGLGETSHADERIVPROC             glGetShaderiv {};
  PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog {};
  PFNGLDELETESHADERPROC            glDeleteShader {};
  PFNGLCREATEPROGRAMPROC           glCreateProgram {};
  PFNGLATTACHSHADERPROC            glAttachShader {};
  PFNGLLINKPROGRAMPROC             glLinkProgram {};
  PFNGLGETPROGRAMIVPROC            glGetProgramiv {};
  PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog {};
  PFNGLUSEPROGRAMPROC              glUseProgram {};
  PFNGLDELETEPROGRAMPROC           glDeleteProgram {};
  PFNGLGETPROGRAMBINARYPROC        glGetProgramBinary {};
  PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays {};
  PFNGLBUFFERDATAPROC              glBufferData {};
  PFNGLGENBUFFERSPROC              glGenBuffers {};
  PFNGLBINDVERTEXARRAYPROC         glBindVertexArray {};
  PFNGLBINDBUFFERPROC              glBindBuffer {};
  PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer {};
  PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray {};
  PFNGLDRAWARRAYSEXTPROC           glDrawArrays {};
  PFNGLSHADERBINARYPROC            glShaderBinary {};
  PFNGLPROGRAMBINARYPROC           glProgramBinary {};
  PFNGLACTIVETEXTUREPROC           glActiveTexture {};
  PFNGLBINDTEXTUREEXTPROC          glBindTexture {};
  PFNGLGENERATEMIPMAPPROC          glGenerateMipmap {};
  PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation {};
  PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation {};
  PFNGLUNIFORM1IPROC               glUniform1i {};
  PFNGLUNIFORM1FPROC               glUniform1f {};

  yage::core::error init(int bits_per_pixel, int width, int height);

  yage::graphics::shader_compiler * create_shader_compiler() override;
  yage::core::error
  destroy_shader_compiler(yage::graphics::shader_compiler * compiler) override;
  yage::graphics::shader * create_shader() override;
  yage::core::error destroy_shader(yage::graphics::shader * shader) override;
  yage::graphics::texture_compiler * create_texture_compiler() override;
  yage::core::error                  destroy_texture_compiler(
                         yage::graphics::texture_compiler * compiler) override;
  yage::core::error destroy_texture(yage::graphics::texture * texture) override;

  yage::core::error clear() override;
  yage::core::error flush() override;

  yage::core::error draw_line(glm::vec2 a, glm::vec2 b) override;
  yage::core::error draw_rect(glm::vec2 position, glm::vec2 dim) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
