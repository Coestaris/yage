/** ========================================================================
 *
 * @file util.hpp
 * @date 15-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <memory>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
namespace yc::gl
{

class mesh_buffer;
class graphics;

/* Variables ================================================================ */
/* Prototypes =============================================================== */
void * gl_lookup(const char * func, bool required);

/** Little helper function to create a cube mesh.
 * Attention: Created mesh, is not managed by any node, so it should be
 * unloaded manually. */
std::shared_ptr<yc::gl::mesh_buffer>
gl_cube(const std::shared_ptr<yc::gl::graphics> & ctx);

}
