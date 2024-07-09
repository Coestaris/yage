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

#include "yagecore/graphics/render_pass.hpp"
#include "yagecore/app/asset_factory.hpp"
#include "yagecore/gl/shader.hpp"
#include "yagecore/platform.hpp"
#include "yagecore/gl/texture.hpp"
#include "yagecore/gl/node.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Make it easier to use the classes. */
using graphics = yc::gl::graphics;
using texture  = yc::gl::texture;
using shader   = yc::gl::shader;
using node     = yc::gl::node;

/* Base class for all render passes. */
class trivial_render_pass : public yc::graphics::render_pass
{
protected:
  std::shared_ptr<shader>   program;
  std::shared_ptr<graphics> ctx;

public:
  virtual ~trivial_render_pass() override = default;
  trivial_render_pass(
        std::string                                     name,
        yc::graphics::pass_type                         type,
        std::shared_ptr<shader>                         shader,
        const std::shared_ptr<yc::graphics::graphics> & graphics)
  : render_pass(std::move(name), type)
  , program(std::move(shader))
  , ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics)) {};
};

/* Performs rendering to the framebuffer. */
class framebuffers_pass : public trivial_render_pass
{
protected:
  framebuffers_pass(
        std::string                             name,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : trivial_render_pass(
          std::move(name),
          yc::graphics::pass_type::VIRTUAL_PASS,
          std::move(shader),
          std::move(graphics)) {};

  void on_object(std::shared_ptr<yc::graphics::node> object) override {
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
        std::string                             name,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : trivial_render_pass(
          std::move(name),
          yc::graphics::pass_type::OBJECT_PASS,
          std::move(shader),
          std::move(graphics)) {};
};

/* No framebuffers are used,
 * rendering is done directly to the screen. */
class final_pass : public trivial_render_pass
{
public:
  final_pass(
        std::string                             name,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : trivial_render_pass(
          std::move(name),
          yc::graphics::pass_type::VIRTUAL_PASS,
          std::move(shader),
          std::move(graphics)) {};

  void on_object(std::shared_ptr<yc::graphics::node> object) override {
    /* Has no sense for current pass. */
  };
};

/* Custom Passes ============================================================ */
class triangle_pass : public final_pass
{
private:
  glm::mat4 proj;
  glm::mat4 view;

  std::shared_ptr<node>    box;
  std::shared_ptr<texture> tex1;
  std::shared_ptr<texture> tex2;

  yc::gl::uniform_int  u_texture1;
  yc::gl::uniform_int  u_texture2;
  yc::gl::uniform_mat4 u_proj;
  yc::gl::uniform_mat4 u_view;
  yc::gl::uniform_mat4 u_model;

  float rotation = 0;

public:
  triangle_pass(
        std::shared_ptr<texture>                tex1,
        std::shared_ptr<texture>                tex2,
        std::shared_ptr<node>                   box,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : final_pass("triangle_pass", std::move(shader), std::move(graphics))
  , box(std::move(box))
  , tex1(std::move(tex1))
  , tex2(std::move(tex2)) {};

  void before_loop() override
  {
    yc::graphics::geometry geometry;
    if (auto window = ctx->get_window().lock())
    {
      window->get_geometry(geometry);
    }

    proj = glm::perspective(
          glm::radians(45.0f),
          geometry.width / static_cast<float>(geometry.height), 0.1f, 100.0f);
    view = glm::identity<glm::mat4>();

    u_texture1 = program->get_uniform<int>("texture1");
    u_texture2 = program->get_uniform<int>("texture2");
    u_proj     = program->get_uniform<glm::mat4>("projection");
    u_view     = program->get_uniform<glm::mat4>("view");
    u_model    = program->get_uniform<glm::mat4>("model");

    program->use();
    u_texture1->set(0);
    u_texture2->set(1);
  }

  void begin() override
  {
    tex2->use(GL_TEXTURE0);
    tex1->use(GL_TEXTURE1);

    program->use();
    box->draw();
  }

  void end() override
  {
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::vector<std::shared_ptr<yc::graphics::render_pass>>
yc::trivial_render_chain(
      const std::shared_ptr<yc::graphics::graphics> & graphics,
      const std::shared_ptr<yc::app::asset_factory> & asset_factory)
{
  std::vector<std::shared_ptr<yc::graphics::render_pass>> passes
        = { std::make_shared<triangle_pass>(
              asset_factory->load_asset<texture>("tex1"),
              asset_factory->load_asset<texture>("tex2"),
              asset_factory->load_asset<node>("box"),
              asset_factory->load_asset<shader>("triangle"), graphics) };

  return passes;
}
