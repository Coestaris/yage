/** ========================================================================
 *
 * @file trivial_render_chain.cpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <utility>
#include <vector>
#include <stdexcept>

#include "GL/gl.h"

#include "graphics/render_pass.hpp"
#include "app/asset_factory.hpp"
#include "platform/graphics/platform.hpp"
#include "platform/graphics/gl/shader.hpp"
#include "platform/graphics/gl/texture.hpp"

#if !defined(YAGE_PLATFORM_X11) && !defined(YAGE_PLATFORM_WIN_GL)              \
      && !defined(YAGE_PLATFORM_WAYLAND)
#error "Pipeline not implemented for this platform"                            \
       "Currently only OpenGL is supported."
#endif

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Base class for all render passes. */
class trivial_render_pass : public yage::graphics::render_pass
{
private:
  yage::app::assets::shader * shader_asset {};

protected:
  yage::platform::graphics::gl::shader *   shader {};
  yage::platform::graphics::gl::graphics * ctx;

public:
  virtual ~trivial_render_pass() override = default;
  trivial_render_pass(
        std::string                 name,
        yage::graphics::pass_type   type,
        yage::app::assets::shader * _shader_asset,
        yage::graphics::graphics *  graphics)
  : render_pass(std::move(name), type)
  , shader_asset(_shader_asset)
  , ctx(dynamic_cast<yage::platform::graphics::gl::graphics *>(graphics)) {};

  void before_loop() override
  {
    set_shader(shader_asset->get_shader());
  }

  virtual void set_shader(yage::graphics::shader * _shader)
  {
    this->shader
          = dynamic_cast<yage::platform::graphics::gl::shader *>(_shader);
  }

  void begin() override
  {
    if (shader_asset->should_update())
      set_shader(shader_asset->get_shader());
  }
};

/* Performs rendering to the framebuffer. */
class framebuffers_pass : public trivial_render_pass
{
protected:
  framebuffers_pass(
        std::string                 name,
        yage::app::assets::shader * shader_asset,
        yage::graphics::graphics *  graphics)
  : trivial_render_pass(
          std::move(name),
          yage::graphics::pass_type::VIRTUAL_PASS,
          shader_asset,
          graphics) {};

  void on_object(yage::graphics::render_primitive * object) override {
    /* Has no sense for current pass. */
  };
};

/* Called for each primitive in the scene.
 * Rendering is done to the framebuffer,
 * but no framebuffer is allocated
 * (it requires an external framebuffer). */
class primitive_pass : public trivial_render_pass
{
protected:
  primitive_pass(
        std::string                 name,
        yage::app::assets::shader * shader_asset,
        yage::graphics::graphics *  graphics)
  : trivial_render_pass(
          std::move(name),
          yage::graphics::pass_type::OBJECT_PASS,
          shader_asset,
          graphics) {};
};

/* No framebuffers are used,
 * rendering is done directly to the screen. */
class final_pass : public trivial_render_pass
{
public:
  final_pass(
        std::string                 name,
        yage::app::assets::shader * shader_asset,
        yage::graphics::graphics *  graphics)
  : trivial_render_pass(
          std::move(name),
          yage::graphics::pass_type::VIRTUAL_PASS,
          shader_asset,
          graphics) {};

  void on_object(yage::graphics::render_primitive * object) override {
    /* Has no sense for current pass. */
  };
};

/* Custom Passes ============================================================ */
class triangle_pass : public final_pass
{
private:
  // clang-format off
  static constexpr const float vertices[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
  };
  static constexpr const int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  // clang-format on

  GLuint VAO, VBO, EBO;

  yage::platform::graphics::gl::texture * tex1;
  yage::platform::graphics::gl::texture * tex2;

public:
  triangle_pass(
        yage::app::assets::shader *  shader_asset,
        yage::app::assets::texture * tex1_asset,
        yage::app::assets::texture * tex2_asset,
        yage::graphics::graphics *   graphics)
  : final_pass("triangle_pass", shader_asset, graphics)
  , tex1(dynamic_cast<yage::platform::graphics::gl::texture *>(
          tex1_asset->get_texture()))
  , tex2(dynamic_cast<yage::platform::graphics::gl::texture *>(
          tex2_asset->get_texture())) {};

  void before_loop() override
  {
    final_pass::before_loop();

    ctx->glGenVertexArrays(1, &VAO);
    ctx->glGenBuffers(1, &VBO);
    ctx->glGenBuffers(1, &EBO);

    ctx->glBindVertexArray(VAO);

    ctx->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    ctx->glBufferData(
          GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ctx->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    ctx->glBufferData(
          GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    ctx->glVertexAttribPointer(
          0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
          reinterpret_cast<void *>(0));
    ctx->glEnableVertexAttribArray(0);
    // color attribute
    ctx->glVertexAttribPointer(
          1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
          reinterpret_cast<void *>(3 * sizeof(float)));
    ctx->glEnableVertexAttribArray(1);
    // texture coord attribute
    ctx->glVertexAttribPointer(
          2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
          reinterpret_cast<void *>(6 * sizeof(float)));
    ctx->glEnableVertexAttribArray(2);

    shader->use();
    shader->set_uniform(shader->get_uniform_location("texture1"), 0);
    shader->set_uniform(shader->get_uniform_location("texture2"), 1);
  }

  void begin() override
  {
    final_pass::begin();

    tex2->use(GL_TEXTURE0);
    tex1->use(GL_TEXTURE1);

    ctx->glBindVertexArray(VAO);
    shader->use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  }

  void end() override
  {
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::vector<yage::graphics::render_pass *>
yage::platform::graphics::trivial_render_chain(
      yage::graphics::graphics *         graphics,
      yage::app::assets::asset_factory * asset_factory)
{
  std::vector<yage::graphics::render_pass *> passes = {
    new triangle_pass(
          asset_factory->load_asset<yage::app::assets::shader>("triangle"),
          asset_factory->load_asset<yage::app::assets::texture>("tex1"),
          asset_factory->load_asset<yage::app::assets::texture>("tex2"),
          graphics),
  };

  return passes;
}
