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
  shader *   program {};
  graphics * ctx;

public:
  virtual ~trivial_render_pass() override = default;
  trivial_render_pass(
        std::string              name,
        yc::graphics::pass_type  type,
        shader *                 shader,
        yc::graphics::graphics * graphics)
  : render_pass(std::move(name), type)
  , program(shader)
  , ctx(dynamic_cast<::graphics *>(graphics)) {};
};

/* Performs rendering to the framebuffer. */
class framebuffers_pass : public trivial_render_pass
{
protected:
  framebuffers_pass(
        std::string name, shader * shader, yc::graphics::graphics * graphics)
  : trivial_render_pass(
          std::move(name),
          yc::graphics::pass_type::VIRTUAL_PASS,
          shader,
          graphics) {};

  void on_object(yc::graphics::node * object) override {
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
        std::string name, shader * program, yc::graphics::graphics * graphics)
  : trivial_render_pass(
          std::move(name),
          yc::graphics::pass_type::OBJECT_PASS,
          program,
          graphics) {};
};

/* No framebuffers are used,
 * rendering is done directly to the screen. */
class final_pass : public trivial_render_pass
{
public:
  final_pass(
        std::string name, shader * program, yc::graphics::graphics * graphics)
  : trivial_render_pass(
          std::move(name),
          yc::graphics::pass_type::VIRTUAL_PASS,
          program,
          graphics) {};

  void on_object(yc::graphics::node * object) override {
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

  texture * tex1;
  texture * tex2;

  yc::gl::uniform_int u_tex1 {};
  yc::gl::uniform_int u_tex2 {};
  yc::gl::uniform_mat4 u_transform {};

  float rotation = 0;

  yc::gl::node * box;

public:
  triangle_pass(
        shader *                 shader,
        texture *                tex1,
        texture *                tex2,
        yc::gl::node *           box,
        yc::graphics::graphics * graphics)
  : final_pass("triangle_pass", shader, graphics)
  , tex1(tex1)
  , tex2(tex2)
  , box(box) {};

  void before_loop() override
  {
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

    program->use();
    u_tex1 = program->get_uniform<int>("texture1");
    u_tex2 = program->get_uniform<int>("texture2");
    u_transform = program->get_uniform<glm::mat4>("transform");

    u_tex1->set(0);
    u_tex2->set(1);
  }

  void begin() override
  {
    tex2->use(GL_TEXTURE0);
    tex1->use(GL_TEXTURE1);

    auto transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    rotation += 0.01f;

    ctx->glBindVertexArray(VAO);
    program->use();
    u_transform->set(transform);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  }

  void end() override
  {
  }
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::vector<yc::graphics::render_pass *> yc::trivial_render_chain(
      yc::graphics::graphics * graphics, yc::app::asset_factory * asset_factory)
{
  std::vector<yc::graphics::render_pass *> passes = { new triangle_pass(
        asset_factory->load_asset<shader>("triangle"),
        asset_factory->load_asset<texture>("tex1"),
        asset_factory->load_asset<texture>("tex2"),
        asset_factory->load_asset<node>("box"), graphics) };

  return passes;
}
