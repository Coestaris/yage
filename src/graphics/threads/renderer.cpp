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
#include "core/io/keyboard.hpp"
#include "graphics/shader_compiler.hpp"
#include "platform/graphics/platform.hpp"
#include "graphics/threads/threads.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static bool compile_shader()
{
  yage::graphics::shader_compiler * compiler
        = yage::platform::graphics::create_shader_compiler();

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

  yage::platform::graphics::destroy_shader_compiler(compiler);

  return true;
}

/* Shared functions ========================================================= */
void yage::graphics::threads::renderer::run()
{
  if (args.window->open(
            const_cast<std::string &>(args.title),
            const_cast<geometry &>(args.geometry), false)
      != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to open window");
    return;
  }

  if (!compile_shader())
  {
    log(LOG_ERROR, "Failed to compile shader");
    return;
  }

  yage::graphics::graphics * graphics = args.window->get_graphics();

  while (true)
  {
    /* Stage 1: Sync with updater before frame */
    profiler->passed(PROF_POINT_RENDERER_PRE_FRAME);
    pre_frame_sync->wait();

    /* Stage 2: Draw the frame using data from previous frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_START);
    int signal = 0;
    if (stop_signal_listener.get_next(signal))
    {
      log(LOG_DEBUG, "Received signal: %d", signal);
      args.window->close();
    }
    if (!args.window->is_open())
    {
      log(LOG_DEBUG, "Window is closed. Exiting renderer worker");
      break;
    }

    float draw_start = yage::core::get_ms();

    graphics->clear();
    for (auto & public_data : game_objects_2)
    {
      graphics->draw_rect(public_data.position, public_data.size);
    }
    graphics->flush();

    /* Stage 3: Sync with updater after frame */
    profiler->passed(PROF_POINT_RENDERER_DRAW_END);
    post_frame_sync->wait();

    /* Stage 4: Copy data from updater to renderer (for next frame) */
    game_objects_2.clear();
    for (auto & object : game_objects_1)
    {
      game_objects_2.push_back(object->data);
    }

    /* Stage 5: Wait for vsync */
    /* Perform moving average.
     * Note that some GL backends may have their own vsync,
     * so this may not work as expected. */
    draw_time = draw_time * 0.9f + (yage::core::get_ms() - draw_start) * 0.1f;
    profiler->passed(PROF_POINT_RENDERER_UPDATE_START);
    if (args.window->update() != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to update window");
      break;
    }

    keeper.call();
  }
}
