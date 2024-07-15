/** ========================================================================
 *
 * @file util.cpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <GL/gl.h>
#if YAGE_PLATFORM_X11
#include <GL/glx.h> /* For glXGetProcAddress */
#elif __WIN32__ || _WIN32
#include <GL/glext.h> /* For wglGetProcAddress */
#endif

#include <functional>
#include <stdexcept>
#include <unordered_map>

#include "yagecore/gl/util.hpp"
#include "yagecore/log.hpp"
#include "yagecore/gl/glfuncs.hpp"
#include "yagecore/gl/node.hpp"
#include "yagecore/graphics/aabb.hpp"
#include "yagecore/graphics/material.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static std::unordered_map<std::string, void *> cache;

/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void * yc::gl::gl_lookup(const char * func, bool required)
{
  if (cache.find(func) != cache.end())
  {
    return cache[func];
  }

#if WIN32
  auto proc = reinterpret_cast<void *>(
        wglGetProcAddress(reinterpret_cast<LPCSTR>(func)));
#else
  auto proc = reinterpret_cast<void *>(
        glXGetProcAddress(reinterpret_cast<const GLubyte *>(func)));
#endif

  if (!proc)
  {
    if (required)
    {
      throw std::runtime_error("Function " + std::string(func) + " not found");
    }
    else
    {
      logging(LOG_ERROR, "Function %s not found", func);
    }

    return nullptr;
  }

  logging(LOG_DEBUG, "Function %s found: %p", func, proc);
  cache[func] = proc;
  return proc;
}

std::shared_ptr<yc::gl::mesh_buffer>
yc::gl::gl_cube(const std::shared_ptr<yc::gl::graphics> & ctx)
{
  /* Cube 1x1x1, centered on origin */
  // clang-format off
  static constexpr GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f, /* 0 */
     0.5f, -0.5f, -0.5f, /* 1 */
     0.5f,  0.5f, -0.5f, /* 2 */
    -0.5f,  0.5f, -0.5f, /* 3 */
    -0.5f, -0.5f,  0.5f, /* 4 */
     0.5f, -0.5f,  0.5f, /* 5 */
     0.5f,  0.5f,  0.5f, /* 6 */
    -0.5f,  0.5f,  0.5f  /* 7 */
  };
  static constexpr GLushort elements[] = {
    /* Front face */
   0, 1, 1, 2, 2, 3, 3, 0,
   /* Back face */
   4, 5, 5, 6, 6, 7, 7, 4,
   /* Connecting edges */
   0, 4, 1, 5, 2, 6, 3, 7
  };
  // clang-format on

  /* Create Vertex Array Object */
  GLuint vao, vbo, ebo;
  yc::gl::glGenVertexArrays(1, &vao);
  yc::gl::glGenBuffers(1, &vbo);
  yc::gl::glGenBuffers(1, &ebo);

  /* Upload data to GPU */
  yc::gl::glBindVertexArray(vao);
  yc::gl::glBindBuffer(GL_ARRAY_BUFFER, vbo);
  yc::gl::glBufferData(
        GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* Upload indices to GPU */
  yc::gl::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  yc::gl::glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  /* Enable and set the vertex attribute pointer */
  yc::gl::glEnableVertexAttribArray(0);
  yc::gl::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  yc::graphics::aabb aabb({ -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f });
  mesh_buffer::indicies_info indicies
        = { .ebo = ebo, .type = GL_UNSIGNED_SHORT, .offset = 0 };
  mesh_buffer::vertices_info vertices_info = {
    .vbo = vbo, .vao = vao, .type = GL_LINES, .count = 24, .aabb = aabb
  };
  return std::make_shared<mesh_buffer>(
        ctx, vertices_info, indicies, yc::graphics::material::basic_material());
}
