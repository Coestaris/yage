/** ========================================================================
 *
 * @file shader_compiler.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <vector>
#include <string>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

#include "yagecore/graphics/shader.hpp"
#include "yagecore/gl/graphics.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yc::graphics::shader
{
private:
  class reloadable
  {
  public:
    virtual ~reloadable() = default;
    virtual void reload() = 0;
  };

public:
  template <typename T> class uniform : public reloadable
  {
  private:
    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, int value)
    {
      ctx->glUniform1i(location, value);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, float value)
    {
      ctx->glUniform1f(location, value);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, glm::vec2 value)
    {
      ctx->glUniform2f(location, value.x, value.y);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, glm::vec3 value)
    {
      ctx->glUniform3f(location, value.x, value.y, value.z);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, glm::vec4 value)
    {
      ctx->glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, glm::mat2 value)
    {
      ctx->glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, glm::mat3 value)
    {
      ctx->glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
    }

    static inline void
    internal_set(yc::gl::graphics * ctx, GLint location, glm::mat4 value)
    {
      ctx->glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }

  protected:
    std::string        name;
    yc::gl::shader *   shader;
    GLint              location {};
    yc::gl::graphics * ctx {};
    T                  prev_value;

  public:
    ~uniform() override = default;

    uniform(std::string name, yc::gl::shader * shader, yc::gl::graphics * ctx)
    : name(std::move(name))
    , shader(shader)
    , ctx(ctx)
    {
      location = shader->get_uniform_location(this->name);
    }

    void reload() override
    {
      int old_loc = location;
      int new_loc = shader->get_uniform_location(name, false);

      logging(
            LOG_INFO, "Reloading uniform %s. %d->%d", name.c_str(), old_loc,
            new_loc);

      location = new_loc;

      set(prev_value);
    }

    inline void set(T value)
    {
      prev_value = value;
      internal_set(ctx, location, value);
    }
  };

protected:
  GLint                                    id;
  yc::gl::graphics *                       ctx;
  std::vector<std::shared_ptr<reloadable>> uniforms;

  yc::error load_from_binary(const std::vector<uint8_t> & binary);
  yc::error compile_binary(std::vector<uint8_t> & binary);

public:
  ~shader() override = default;
  shader(
        std::string              name,
        yc::app::asset_cacher *  cacher,
        yc::graphics::graphics * graphics)
  : yc::graphics::shader(std::move(name), cacher, graphics)
  , ctx(dynamic_cast<yc::gl::graphics *>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

  /* GL specific sfuff */
  GLint get_uniform_location(const std::string & name, bool required = true);
  GLint get_attribute_location(const std::string & name, bool required = true);

  inline void use()
  {
    ctx->glUseProgram(id);
  }

  template <typename T>
  std::shared_ptr<uniform<T>> get_uniform(std::string name)
  {
    auto u = std::make_shared<uniform<T>>(name, this, ctx);
    uniforms.push_back(u);
    return u;
  }
};

using uniform_int   = std::shared_ptr<shader::uniform<int>>;
using uniform_float = std::shared_ptr<shader::uniform<float>>;
using uniform_vec2  = std::shared_ptr<shader::uniform<glm::vec2>>;
using uniform_vec3  = std::shared_ptr<shader::uniform<glm::vec3>>;
using uniform_vec4  = std::shared_ptr<shader::uniform<glm::vec4>>;
using uniform_mat2  = std::shared_ptr<shader::uniform<glm::mat2>>;
using uniform_mat3  = std::shared_ptr<shader::uniform<glm::mat3>>;
using uniform_mat4  = std::shared_ptr<shader::uniform<glm::mat4>>;

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}
