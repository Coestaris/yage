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
#include "glplatform.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader : public yc::graphics::shader,
               public std::enable_shared_from_this<shader>
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
    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          int                                       value)
    {
      yc::gl::glUniform1i(location, value);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          float                                     value)
    {
      yc::gl::glUniform1f(location, value);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          glm::vec2                                 value)
    {
      yc::gl::glUniform2f(location, value.x, value.y);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          glm::vec3                                 value)
    {
      yc::gl::glUniform3f(location, value.x, value.y, value.z);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          glm::vec4                                 value)
    {
      yc::gl::glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          glm::mat2                                 value)
    {
      yc::gl::glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          glm::mat3                                 value)
    {
      yc::gl::glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
    }

    static inline void internal_set(
          const std::shared_ptr<yc::gl::graphics> & ctx,
          GLint                                     location,
          glm::mat4                                 value)
    {
      yc::gl::glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }

  protected:
    std::shared_ptr<yc::gl::graphics> ctx;
    std::weak_ptr<yc::gl::shader>     shader;

    std::string name;
    GLint       location {};
    T           prev_value;

  public:
    ~uniform() override = default;

    uniform(
          std::string                       name,
          std::shared_ptr<yc::gl::shader>   shader,
          std::shared_ptr<yc::gl::graphics> ctx)
    : ctx(std::move(ctx))
    , shader(std::move(shader))
    , name(std::move(name))
    {
      location = this->shader.lock()->get_uniform_location(this->name);
    }

    void reload() override
    {
      int old_loc = location;
      int new_loc = shader.lock()->get_uniform_location(name);

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
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  GLint                                    id {};
  std::vector<std::shared_ptr<reloadable>> uniforms {};

  yc::error load_from_binary(const std::vector<uint8_t> & binary);
  yc::error compile_binary(std::vector<uint8_t> & binary);

public:
  ~shader() override = default;
  explicit shader(
        std::string                                     name,
        std::shared_ptr<yc::app::asset_cacher>          cacher,
        const std::shared_ptr<yc::graphics::graphics> & graphics)
  : yc::graphics::shader(std::move(name), std::move(cacher), graphics)
  , ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics))
  {
  }

  yc::error load() override;
  void      unload() override;

  /* GL specific sfuff */
  GLint get_uniform_location(const std::string & name, bool required = true);
  GLint get_attribute_location(const std::string & name, bool required = true);

  inline void use()
  {
    yc::gl::glUseProgram(id);
  }

  template <typename T>
  std::shared_ptr<uniform<T>> get_uniform(std::string name)
  {
    auto u = std::make_shared<uniform<T>>(
          std::move(name), shared_from_this(), ctx);
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
