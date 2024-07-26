/** ========================================================================
 *
 * @file shader.hpp
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
#include "yagecore/gl/util.hpp"
#include "yagecore/gl/glfuncs.hpp"

namespace yc::gl
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class shader final : public yc::graphics::shader,
                     public std::enable_shared_from_this<shader>
{
  static inline void internal_set(GLint location, int value);
  static inline void internal_set(GLint location, float value);
  static inline void internal_set(GLint location, glm::vec2 value);
  static inline void internal_set(GLint location, glm::vec3 value);
  static inline void internal_set(GLint location, glm::vec4 value);
  static inline void internal_set(GLint location, glm::mat2 value);
  static inline void internal_set(GLint location, glm::mat3 value);
  static inline void internal_set(GLint location, glm::mat4 value);

  class reloadable
  {
  public:
    virtual ~    reloadable() = default;
    virtual void reload()     = 0;
  };

public:
  template <typename T> class uniform : public reloadable
  {
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
          std::shared_ptr<yc::gl::graphics> ctx);

    void reload() override;

    /* Warning: You should use shader by HAND */
    void set(T value);
  };

private:
  GLint get_uniform_location(const std::string & name, bool required = true);
  GLint get_attribute_location(const std::string & name, bool required = true);

protected:
  /* Shortcut for GL specific graphics */
  std::shared_ptr<yc::gl::graphics> ctx;

  GLint                                    id {};
  std::vector<std::shared_ptr<reloadable>> uniforms {};

  yc::error load_from_binary(const std::vector<std::byte> & binary);
  yc::error compile_binary(std::vector<std::byte> & binary);

public:
  ~        shader() override = default;
  explicit shader(
        std::string                                     name,
        std::shared_ptr<yc::app::asset_cacher>          cacher,
        const std::shared_ptr<yc::graphics::graphics> & graphics);

  yc::error load() override;
  void      unload() override;

  void use() const;

  template <typename T>
  std::shared_ptr<uniform<T>> get_uniform(std::string name);
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
