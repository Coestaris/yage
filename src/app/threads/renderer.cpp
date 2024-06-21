/** ========================================================================
 *
 * @file renderer.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>

#include "renderer.hpp"
#include "core/time.hpp"
#include "graphics/shader_compiler.hpp"
#include "platform/graphics/platform.hpp"
#include "app/application.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yage::app::threads::renderer::run()
{
  if (app->window->open() != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to open window");
    return;
  }

  graphics = app->window->get_graphics();
  if (!graphics)
  {
    log(LOG_ERROR, "Unable to get graphics from window");
    return;
  }

  if (!compile_shader())
  {
    log(LOG_ERROR, "Failed to compile shader");
    return;
  }

  while (true)
  {
    /* Stage 1: Sync with updater before frame */
    profiler->start_cycle();
    profiler->passed(PROF_POINT_RENDERER_PRE_FRAME);
    app->pre_frame_sync.wait();

    /* Stage 2: Draw the frame using data from previous frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_START);
    int signal = 0;
    if (stop_signal_listener.get_next(signal))
    {
      log(LOG_DEBUG, "Received signal: %d", signal);
      app->window->close();
    }
    if (!app->window->is_open())
    {
      log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }

    graphics->clear();
    for (auto & public_data : app->objects_renderer)
    {
      graphics->draw_rect(public_data.position, public_data.size);
    }
    instances_count = app->objects_renderer.size();
    graphics->flush();

    /* Stage 3: Sync with updater after frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_END);
    app->post_frame_sync.wait();

    /* Stage 4: Copy data from updater to renderer (for next frame) */
    app->objects_renderer.clear();
    for (auto & object : app->objects_logic)
    {
      app->objects_renderer.push_back(object->data);
    }

    /* Stage 5: Wait for vsync */
    profiler->passed(PROF_POINT_RENDERER_UPDATE_START);
    if (app->window->update() != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to update window");
      break;
    }

    keeper.call();
  }
}

bool yage::app::threads::renderer::compile_shader()
{
  yage::graphics::shader_compiler * compiler
        = graphics->create_shader_compiler();

  /* Read shader source */
  //  std::string vertex_shader_path   = resources_directory + "/triangle.vsh";
  //  std::string fragment_shader_path = resources_directory + "/triangle.fsh";

  std::string vertex_shader_path
        = R"(/home/taris/work/yage/resources/triangle.vsh)";
  std::string fragment_shader_path
        = R"(/home/taris/work/yage/resources/triangle.fsh)";

  log(LOG_INFO, "Reading vertex shader source from: %s",
      vertex_shader_path.c_str());
  std::ifstream file(vertex_shader_path, std::ios::in);
  if (!file.is_open())
  {
    log(LOG_ERROR, "Failed to open file: %s", vertex_shader_path.c_str());
    return false;
  }
  std::string vertex_shader_source(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
  file.close();

  log(LOG_INFO, "Reading fragment shader source from: %s",
      fragment_shader_path.c_str());
  file.open(fragment_shader_path, std::ios::in);
  if (!file.is_open())
  {
    log(LOG_ERROR, "Failed to open file: %s", fragment_shader_path.c_str());
    return false;
  }
  std::string fragment_shader_source(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
  file.close();

  if (compiler->add_shader(
            vertex_shader_source, yage::graphics::shader_type::VERTEX_SHADER)
      != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to add vertex shader");
    return false;
  }

  if (compiler->add_shader(
            fragment_shader_source,
            yage::graphics::shader_type::FRAGMENT_SHADER)
      != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to add fragment shader");
    return false;
  }

  std::vector<yage::graphics::shader_compiler::compile_message> messages;
  bool has_errors = false;
  if (compiler->compile(messages) != yage::core::error::OK)
  {
    has_errors = true;
  }

  if (!messages.empty())
  {
    log(LOG_INFO, "Shader messages:");
  }

  for (auto & message : messages)
  {
    yage::core::log_level log_level = LOG_INFO;
    if (message.type
        == yage::graphics::shader_compiler::compile_message::TYPE_ERROR)
    {
      log_level  = LOG_ERROR;
      has_errors = true;
    }
    else if (
          message.type
          == yage::graphics::shader_compiler::compile_message::TYPE_WARNING)
    {
      log_level = LOG_WARN;
    }
    else if (
          message.type
          == yage::graphics::shader_compiler::compile_message::TYPE_INFO)
    {
      log_level = LOG_INFO;
    }

    log(log_level, " [%d] > %s ", message.line, message.message.c_str());
  }

  if (has_errors)
  {
    log(LOG_ERROR, "Failed to compile shaders");
    return false;
  }

  graphics->destroy_shader_compiler(compiler);

  return true;
}
