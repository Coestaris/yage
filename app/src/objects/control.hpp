/** ========================================================================
 *
 * @file control.hpp
 * @date 11-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <memory>
#include <utility>
#include "yagecore/app/object.hpp"

extern glm::mat4 app_view;

namespace app::objects
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */

class control : public yc::app::object
{
  glm::vec3 camera_pos   = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 camera_dir   = glm::vec3(0.0f, 0.0f, 1.0f);
  float     camera_yaw   = 0.0f;
  float     camera_pitch = 0.0f;

  glm::mat4 view        = glm::mat4(1.0f);
  glm::vec2 prev_mouse  = glm::vec2(0.0f);
  bool      recalculate = true;

public:
  ~        control() override = default;
  explicit control(std::shared_ptr<yc::app::application> app)
  : yc::app::object(std::move(app)) {};

  void
  on_ready(const std::shared_ptr<yc::app::asset_factory> & factory) override {};

  void update(yc::milliseconds delta) override
  {
    if (app->get_input()->key_pressed(yc::key::ESCAPE))
    {
      app->stop();
    }

    auto mouse_pos = app->get_input()->mouse_position();
    if (app->get_input()->mouse_pressed(yc::mouse::LEFT))
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

    if (app->get_input()->key_pressed(yc::key::W))
    {
      camera_pos += 0.01f * camera_dir * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::S))
    {
      camera_pos -= 0.01f * camera_dir * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::A))
    {
      camera_pos -= glm::normalize(
                          glm::cross(camera_dir, glm::vec3(0.0f, 1.0f, 0.0f)))
                    * 0.01f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::D))
    {
      camera_pos += glm::normalize(
                          glm::cross(camera_dir, glm::vec3(0.0f, 1.0f, 0.0f)))
                    * 0.01f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::SPACE))
    {
      camera_pos += glm::vec3(0.0f, 0.01f, 0.0f) * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::SHIFT_L))
    {
      camera_pos -= glm::vec3(0.0f, 0.01f, 0.0f) * delta;
      recalculate = true;
    }

    if (!recalculate)
      return;

    app_view = glm::lookAt(
          camera_pos, camera_pos + camera_dir, glm::vec3(0.0f, 1.0f, 0.0f));
    recalculate = false;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}