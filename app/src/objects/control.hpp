/** ========================================================================
 *
 * @file control.hpp
 * @date 11-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include "yagecore/app/object.hpp"
#include "app.hpp"

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

  std::shared_ptr<app::application> ctx;

public:
  ~        control() override = default;
  explicit control(std::shared_ptr<app::application> app)
  : yc::app::object(std::reinterpret_pointer_cast<yc::app::application>(app))
  , ctx(std::move(app)) {};

  void on_update(yc::milliseconds delta) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}