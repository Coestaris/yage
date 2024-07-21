/** ========================================================================
 *
 * @file object.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/app/object.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::app::object::object(std::shared_ptr<yc::app::application> app)
: app(std::move(app)) {};

void yc::app::object::update()
{
  yc::milliseconds delta;
  yc::milliseconds time = yc::get_ms();
  if (last_update == 0)
  {
    last_update = time;
    delta       = 0;
  }
  else
  {
    delta       = time - last_update;
    last_update = time;
  }

  on_update(delta);
}

void yc::app::object::set_visible(bool visible)
{
  renderer_data.renderable.visible = visible;
}

void yc::app::object::set_node(std::shared_ptr<yc::graphics::node> node)
{
  renderer_data.renderable.node = std::move(node);
}

void yc::app::object::set_transform(const glm::mat4 & transform)
{
  renderer_data.renderable.transform = transform;
  if (renderer_data.renderable.node)
  {
    renderer_data.renderable.aabb_transform
          = transform
            * renderer_data.renderable.node->get_aabb().get_transform();
  }
}