/** ========================================================================
 *
 * @file platform.hpp
 * @date 06-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "graphics/window.hpp"
#include "graphics/shader_compiler.hpp"
#include "graphics/shader.hpp"

namespace yage::platform::graphics
{

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Prototypes =============================================================== */
yage::graphics::window * create_window(yage::graphics::settings & settings);
void                     destroy_window(yage::graphics::window * window);

yage::graphics::shader_compiler * create_shader_compiler();
void destroy_shader_compiler(yage::graphics::shader_compiler * compiler);

yage::graphics::shader * create_shader();
void                     destroy_shader(yage::graphics::shader * shader);

}
