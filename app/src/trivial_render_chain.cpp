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

#include <yagecore/app/object.hpp>

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
  ~trivial_render_pass() override = default;
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

  void on_node(const yc::app::object::renderer_data & object) override {
    /* Has no sense for current pass. */
  };
  void on_mesh(std::shared_ptr<yc::graphics::material> material) override {
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

  void on_node(const yc::app::object::renderer_data & object) override {
    /* Has no sense for current pass. */
  };
  void on_mesh(std::shared_ptr<yc::graphics::material> material) override {
    /* Has no sense for current pass. */
  };
};

/* Custom Passes ============================================================ */
static glm::mat4 view = glm::mat4(1.0f);
glm::mat4        app_view;

void yc::after_render(void)
{
  view = app_view;
}

class basic_geometry_pass : public primitive_pass
{
  glm::mat4 proj;

  yc::gl::uniform_mat4 u_proj;
  yc::gl::uniform_mat4 u_view;
  yc::gl::uniform_mat4 u_model;

  float rotation = 0;

public:
  basic_geometry_pass(
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : primitive_pass("basic_geometry", std::move(shader), std::move(graphics)) {};

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

    u_proj  = program->get_uniform<glm::mat4>("projection");
    u_view  = program->get_uniform<glm::mat4>("view");
    u_model = program->get_uniform<glm::mat4>("model");

    u_proj->set(proj);
  }

  void begin() override
  {
    program->use();
    u_view->set(view);
  }

  void on_mesh(std::shared_ptr<yc::graphics::material> material) override
  {
  }

  void on_node(const yc::app::object::renderer_data & object) override
  {
    u_model->set(object.transform);
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
  const auto layout = {
    node::attr_layout_item(node::attr_layout_item::type::POSITION, 0),
    node::attr_layout_item(node::attr_layout_item::type::NORMAL, 1, false),
    node::attr_layout_item(node::attr_layout_item::type::TEXCOORD, 2, false),
  };
  node::set_layout(layout);

  std::vector<std::shared_ptr<yc::graphics::render_pass>> passes
        = { std::make_shared<basic_geometry_pass>(
              asset_factory->load_asset<shader>("basic_geometry"), graphics) };

  return passes;
}
