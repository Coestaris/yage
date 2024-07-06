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

  yage::graphics::shader * create_shader(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics) override;
  yage::core::error destroy_shader(yage::graphics::shader * shader) override;

  yage::graphics::texture * create_texture(
        std::string                    name,
        yage::graphics::asset_cacher * cacher,
        yage::graphics::graphics *     graphics)
        override;
  yage::core::error destroy_texture(yage::graphics::texture * texture)
        override;


  yage::core::error clear() override;
  yage::core::error flush() override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
