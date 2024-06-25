/** ========================================================================
 *
 * @file shader.cpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <fstream>
#include "app/assets/shader.hpp"
#include "core/log.hpp"
#include "core/hash.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yage::core::error yage::app::assets::shader::load()
{
  if (is_loaded())
  {
    return yage::core::error::OK;
  }
  live = true;

  yage::graphics::shader_compiler * compiler
        = graphics->create_shader_compiler();

  for (auto & source : src)
  {
    yage::core::error err = compiler->add_shader(source.second, source.first);
    if (err != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to add shader source: %s", source.second.c_str());
      return err;
    }
  }

  std::string          cache_key = get_cache_key();
  std::vector<uint8_t> data;
  if (cacher->get_cached_asset(cache_key, data) == yage::core::error::OK)
  {
    log(LOG_DEBUG, "Loading shader from cache: %s (%zu bytes)", cache_key.c_str(), data.size());
  }
  else
  {
    log(LOG_DEBUG, "Compiling shader: %s", cache_key.c_str());

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
      log(LOG_ERROR, "Failed to compile shader %s", cache_key.c_str());
      return yage::core::error::INVALID_STATE;
    }

    if (compiler->get_binary(data) != yage::core::error::OK)
    {
      log(LOG_ERROR, "Failed to get binary representation of shader of %s",
          cache_key.c_str());
      return yage::core::error::INVALID_STATE;
    }

    cacher->cache_asset(cache_key, data);
  }

  graphics->destroy_shader_compiler(compiler);

  program = graphics->create_shader();
  if (!program)
  {
    log(LOG_ERROR, "Failed to create shader program");
    return yage::core::error::INVALID_STATE;
  }

  if (program->set_binary(data) != yage::core::error::OK)
  {
    log(LOG_ERROR, "Failed to set binary representation of shader");
    return yage::core::error::INVALID_STATE;
  }

  return yage::core::OK;
}

void yage::app::assets::shader::unload()
{
  if (!is_loaded())
  {
    return;
  }

  program->destroy();
  program = nullptr;
  live    = false;
  loaded  = false;
}
std::string yage::app::assets::shader::get_cache_key()
{
  yage::core::fast_hasher hasher;
  for (auto & source : src)
  {
    hasher.add(source.second);
    hasher.add(std::to_string(source.first));
  }

  return "shader" + name + hasher.as_string();
}
