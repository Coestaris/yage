/** ========================================================================
 *
 * @file trivial_render_chain.cpp
 * @date 28-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <vector>
#include "graphics/render_pass.hpp"
#include "platform/graphics/platform.hpp"

#if !defined(YAGE_PLATFORM_X11) && !defined(YAGE_PLATFORM_WIN_GL)              \
      && !defined(YAGE_PLATFORM_WAYLAND)
#error "Pipeline not implemented for this platform"                            \
       "Currently only OpenGL is supported."
#endif

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Base class for all render passes. */
class trivial_render_pass : public yage::graphics::render_pass
{
};

/* Performs rendering to the framebuffer. */
class framebuffers_pass : public trivial_render_pass
{
};

/* Called for each primitive in the scene.
 * Rendering is done to the framebuffer,
 * but no framebuffer is allocated
 * (it requires an external framebuffer). */
class primitive_pass : public trivial_render_pass
{
};

/* Called for each primitive in the scene.
 * Rendering is done to the framebuffer.
 * Allocates a framebuffer. */
class primitive_framebuffer_pass : public framebuffers_pass,
                                   public primitive_pass
{
};

/* No framebuffers are used,
 * rendering is done directly to the screen. */
class final_pass : public trivial_render_pass
{
};

/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
std::vector<yage::graphics::render_pass *>
yage::platform::graphics::trivial_render_chain(
      yage::graphics::graphics *         graphics,
      yage::app::assets::asset_factory * asset_factory)
{
  std::vector<yage::graphics::render_pass *> passes;

  return passes;
}
