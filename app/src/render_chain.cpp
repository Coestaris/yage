/** ========================================================================
 *
 * @file render_chain.cpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <utility>
#include <vector>
#include <stdexcept>

#include "GL/gl.h"

#include "app.hpp"
#include "yagecore/graphics/render_pass.hpp"
#include "yagecore/app/asset_factory.hpp"
#include "yagecore/platform.hpp"

#include "yagecore/gl/shader.hpp"
#include "yagecore/gl/texture.hpp"
#include "yagecore/gl/node.hpp"
#include "yagecore/gl/font.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <cinttypes>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Make it easier to use the classes. */
using graphics = yc::gl::graphics;
using texture  = yc::gl::texture;
using shader   = yc::gl::shader;
using node     = yc::gl::node;
using font     = yc::gl::font;

/* Base class for all render passes. */
class trivial_render_pass : public yc::graphics::render_pass
{
protected:
  std::shared_ptr<shader>           program;
  std::shared_ptr<app::application> app;
  std::shared_ptr<graphics>         ctx;

public:
  ~trivial_render_pass() override = default;
   trivial_render_pass(
         std::string                                     name,
         yc::graphics::render_pass::type                 type,
         std::shared_ptr<yc::app::application>           app,
         std::shared_ptr<shader>                         shader,
         const std::shared_ptr<yc::graphics::graphics> & graphics)
  : render_pass(std::move(name), type)
  , program(std::move(shader))
  , app(std::reinterpret_pointer_cast<app::application>(app))
  , ctx(std::dynamic_pointer_cast<yc::gl::graphics>(graphics)) {};
};

/* Performs rendering to the framebuffer. */
class framebuffers_pass : public trivial_render_pass
{
protected:
  framebuffers_pass(
        std::string                             name,
        std::shared_ptr<yc::app::application>   app,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : trivial_render_pass(
          std::move(name),
          type::VIRTUAL_PASS,
          std::move(app),
          std::move(shader),
          std::move(graphics)) {};

  bool on_node(const yc::app::object::renderable & object) override
  {
    /* Has no sense for current pass. */
    return false;
  };

  bool on_mesh(
        const yc::graphics::mesh_buffer &               mesh,
        const std::shared_ptr<yc::graphics::material> & material) override
  {
    /* Has no sense for current pass. */
    return false;
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
        std::shared_ptr<yc::app::application>   app,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : trivial_render_pass(
          std::move(name),
          type::OBJECT_PASS,
          std::move(app),
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
        std::shared_ptr<yc::app::application>   app,
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : trivial_render_pass(
          std::move(name),
          type::VIRTUAL_PASS,
          std::move(app),
          std::move(shader),
          std::move(graphics)) {};

  bool on_node(const yc::app::object::renderable & object) override
  {
    /* Has no sense for current pass. */
    return false;
  };

  bool on_mesh(
        const yc::graphics::mesh_buffer &               mesh,
        const std::shared_ptr<yc::graphics::material> & material) override
  {
    /* Has no sense for current pass. */
    return false;
  };
};

/* Custom Passes ============================================================ */
class basic_geometry_pass : public primitive_pass
{
  yc::gl::uniform_mat4 u_proj;
  yc::gl::uniform_mat4 u_view;
  yc::gl::uniform_mat4 u_model;

  float rotation = 0;

public:
  basic_geometry_pass(
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::app::application>   app,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : primitive_pass(
          "basic_geometry",
          std::move(app),
          std::move(shader),
          std::move(graphics)) {};

  void before_loop() override
  {
    yc::graphics::geometry geometry;
    if (auto window = ctx->get_window().lock())
    {
      window->get_geometry(geometry);
    }

    app->set_proj_matrix(glm::perspective(
          glm::radians(45.0f),
          geometry.width / static_cast<float>(geometry.height), 0.1f, 100.0f));

    program->use();
    u_proj  = program->get_uniform<glm::mat4>("projection");
    u_view  = program->get_uniform<glm::mat4>("view");
    u_model = program->get_uniform<glm::mat4>("model");

    program->use();
    u_proj->set(app->get_proj_matrix());
  }

  void on_begin() override
  {
    /* Copy the view matrix from the main thread */
    app->sync_view_matrix();

    program->use();
    u_view->set(app->get_view_matrix());
  }

  bool on_mesh(
        const yc::graphics::mesh_buffer &               mesh,
        const std::shared_ptr<yc::graphics::material> & material) override
  {
    return true;
  }

  bool on_node(const yc::app::object::renderable & object) override
  {
    u_model->set(object.transform);
    return true;
  }

  void on_end() override
  {
  }
};

class draw_aabb_pass : public primitive_pass,
                       public std::enable_shared_from_this<draw_aabb_pass>
{
  std::shared_ptr<yc::gl::mesh_buffer> cube;

  yc::gl::uniform_mat4 u_proj;
  yc::gl::uniform_mat4 u_view;
  yc::gl::uniform_mat4 u_model;
  yc::gl::uniform_vec4 u_color;

public:
  draw_aabb_pass(
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::app::application>   app,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : primitive_pass(
          "draw_aabb",
          std::move(app),
          std::move(shader),
          std::move(graphics)) {};

  void before_loop() override
  {
    cube = yc::gl::gl_cube(ctx);

    u_model = program->get_uniform<glm::mat4>("model");
    u_proj  = program->get_uniform<glm::mat4>("projection");
    u_view  = program->get_uniform<glm::mat4>("view");
    u_color = program->get_uniform<glm::vec4>("color");

    program->use();
    u_proj->set(app->get_proj_matrix());
    u_color->set(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  }

  void on_begin() override
  {
    program->use();
    u_view->set(app->get_view_matrix());
  }

  bool on_mesh(
        const yc::graphics::mesh_buffer &               mesh,
        const std::shared_ptr<yc::graphics::material> & material) override
  {
    return true;
  }

  bool on_node(const yc::app::object::renderable & object) override
  {
    u_model->set(object.aabb_transform);
    cube->draw(shared_from_this());

    return false;
  }

  void on_end() override
  {
  }
};

class custom_char_render : public yc::gl::char_renderer
{
  std::shared_ptr<app::application> app;
  std::shared_ptr<shader>           program;

  yc::gl::uniform_mat4 u_proj;
  yc::gl::uniform_mat4 u_model;
  yc::gl::uniform_vec4 u_color;
  yc::gl::uniform_int  u_atlas;

public:
  ~custom_char_render() override = default;
   custom_char_render(
         std::shared_ptr<app::application> app, std::shared_ptr<shader> shader)
  : app(app)
  , program(shader)
  {
    u_proj  = program->get_uniform<glm::mat4>("projection");
    u_model = program->get_uniform<glm::mat4>("model");
    u_color = program->get_uniform<glm::vec4>("color");
    u_atlas = program->get_uniform<int>("atlas");

    auto geometry = app->get_geometry();

    shader->use();
    u_proj->set(glm::ortho(
          0.0f, static_cast<float>(geometry.width), 0.0f,
          static_cast<float>(geometry.height)));
    u_atlas->set(0);
  };

  void begin(GLuint vao, glm::vec4 color, GLuint atlas_id) override
  {
    program->use();
    u_color->set(color);
    yc::gl::glBindVertexArray(vao);
    yc::gl::glBindTexture(GL_TEXTURE_2D, atlas_id);
  }

  void end() override
  {
    yc::gl::glBindVertexArray(0);
  }

  void
  on_char(const yc::graphics::font::char_cache & c, glm::mat4 model) override
  {
    u_model->set(model);
    yc::gl::glDrawElements(
          GL_TRIANGLES, c.__vert_count, GL_UNSIGNED_INT,
          reinterpret_cast<void *>(c.__vert_offset));
  }
};

class draw_hud_pass : public final_pass,
                      public std::enable_shared_from_this<draw_hud_pass>
{
  std::shared_ptr<font>                   f;
  uint16_t                                height = 0;
  std::unordered_map<size_t, std::string> pass_names;

public:
  draw_hud_pass(
        std::shared_ptr<shader>                 shader,
        std::shared_ptr<yc::app::application>   app,
        std::shared_ptr<font>                   font,
        std::shared_ptr<yc::graphics::graphics> graphics)
  : final_pass(
          "draw_hud", std::move(app), std::move(shader), std::move(graphics))
  , f(std::move(font)) {};

  void before_loop() override
  {
    f->set_char_renderer(std::make_shared<custom_char_render>(app, program));

    for (size_t i = 0; i < app->stats.get_passes_count(); i++)
    {
      pass_names[i] = app->stats.get_pass_name(i);
    }

    height = app->get_geometry().height;
  }

  void on_begin() override
  {
    char text[256];
#define LINE_HEIGHT 15
#define OFFSET      20
    snprintf(
          text, sizeof(text), "Renderer: %.1f. %.1f, %.1f", app->stats.get_fps(),
          app->stats.get_draw_time(), app->stats.get_copy_time());
    f->draww(text, { 10, height - OFFSET }, glm::vec4(1.0f, 0.8f, 1.0f, 1.0f));

    snprintf(
          text, sizeof(text),
          "Logic: %" PRIu64 ", %.1f, %.1f, %.1f, %.1f, %.1f",
          app->stats.get_instances_count(), app->stats.get_update_time(),
          app->stats.get_particles_time(), app->stats.get_physics_time(),
          app->stats.get_ai_time(), app->stats.get_sound_time());
    f->draww(
          text, { 10, height - OFFSET - LINE_HEIGHT },
          glm::vec4(1.0f, 0.8f, 1.0f, 1.0f));

    for (size_t i = 0; i < app->stats.get_passes_count(); i++)
    {
      snprintf(
            text, sizeof(text), "%s: %.1f, %" PRIu64 ", %" PRIu64 "k",
            pass_names[i].c_str(), app->stats.get_pass_info(i).draw_time,
            app->stats.get_pass_info(i).drawn_meshes,
            app->stats.get_pass_info(i).drawn_triangles / 1000);
      f->draww(
            text, { 10, height - OFFSET - LINE_HEIGHT * (i + 3) },
            glm::vec4(1.0f, 1.0f, 0.8f, 1.0f));
    }
  }

  void on_end() override
  {
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::vector<std::shared_ptr<yc::graphics::render_pass>>
app::application::create_render_chain(
      const std::shared_ptr<yc::graphics::graphics> & graphics,
      const std::shared_ptr<yc::app::asset_factory> & asset_factory)
{
  const auto layout = {
    node::attr_layout_item(node::attr_layout_item::type::POSITION, 0),
    node::attr_layout_item(node::attr_layout_item::type::NORMAL, 1, false),
    node::attr_layout_item(node::attr_layout_item::type::TEXCOORD, 2, false),
  };
  node::set_layout(layout);

  std::vector<std::shared_ptr<yc::graphics::render_pass>> passes = {
    std::make_shared<basic_geometry_pass>(
          asset_factory->load_asset<shader>("basic_geometry"),
          shared_from_this(), graphics),
    std::make_shared<draw_aabb_pass>(
          asset_factory->load_asset<shader>("line"), shared_from_this(),
          graphics),
    std::make_shared<draw_hud_pass>(
          asset_factory->load_asset<shader>("text"), shared_from_this(),
          asset_factory->load_asset<font>("SpaceMono"), graphics),
  };

  return passes;
}