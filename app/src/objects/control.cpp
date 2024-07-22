/** ========================================================================
 *
 * @file control.cpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "control.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void app::objects::control::on_update(yc::milliseconds delta)
{
  if (ctx->get_input()->key_pressed(yc::key::ESCAPE))
  {
    ctx->stop();
  }

  auto mouse_pos = ctx->get_input()->mouse_position();
  if (ctx->get_input()->mouse_pressed(yc::mouse::LEFT))
  {
    auto mouse_delta = mouse_pos - prev_mouse;
    camera_yaw += mouse_delta.x * 0.1f;
    camera_pitch += mouse_delta.y * 0.1f;

    camera_dir = glm::vec3(
          cos(glm::radians(camera_pitch)) * sin(glm::radians(camera_yaw)),
          sin(glm::radians(camera_pitch)),
          cos(glm::radians(camera_pitch)) * cos(glm::radians(camera_yaw)));
    camera_dir = glm::normalize(camera_dir);

    recalculate = true;
  }
  prev_mouse = mouse_pos;

  if (ctx->get_input()->key_pressed(yc::key::W))
  {
    camera_pos += 0.01f * camera_dir * delta;
    recalculate = true;
  }
  if (ctx->get_input()->key_pressed(yc::key::S))
  {
    camera_pos -= 0.01f * camera_dir * delta;
    recalculate = true;
  }
  if (ctx->get_input()->key_pressed(yc::key::A))
  {
    camera_pos
          -= glm::normalize(glm::cross(camera_dir, glm::vec3(0.0f, 1.0f, 0.0f)))
             * 0.01f * delta;
    recalculate = true;
  }
  if (ctx->get_input()->key_pressed(yc::key::D))
  {
    camera_pos
          += glm::normalize(glm::cross(camera_dir, glm::vec3(0.0f, 1.0f, 0.0f)))
             * 0.01f * delta;
    recalculate = true;
  }
  if (ctx->get_input()->key_pressed(yc::key::SPACE))
  {
    camera_pos += glm::vec3(0.0f, 0.01f, 0.0f) * delta;
    recalculate = true;
  }
  if (ctx->get_input()->key_pressed(yc::key::SHIFT_L))
  {
    camera_pos -= glm::vec3(0.0f, 0.01f, 0.0f) * delta;
    recalculate = true;
  }

  if (!recalculate)
    return;

  ctx->set_view_matrix(glm::lookAt(
        camera_pos, camera_pos + camera_dir, glm::vec3(0.0f, 1.0f, 0.0f)));

  recalculate = false;
}