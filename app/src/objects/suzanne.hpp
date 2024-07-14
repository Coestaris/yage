/** ========================================================================
 *
 * @file suzanne.hpp
 * @date 11-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <memory>
#include <utility>
#include "yagecore/app/object.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

namespace app::objects
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class suzanne : public yc::app::object
{
private:
  glm::mat4 start_pos = 0;

  float yaw {};
  float pitch {};
  float roll {};

  bool recalculate = true;

public:
  ~        suzanne() override = default;
  explicit suzanne(std::shared_ptr<yc::app::application> app)
  : yc::app::object(std::move(app))
  {
    start_pos = glm::translate(
          glm::mat4(1.0f),
          glm::vec3(
                glm::linearRand(-8.0f, 8.0f), glm::linearRand(-8.0f, 8.0f),
                glm::linearRand(-8.0f, 8.0f)));
    start_pos = glm::scale(start_pos, glm::vec3(0.5f));
  };

  void
  on_ready(const std::shared_ptr<yc::app::asset_factory> & factory) override
  {
    set_node(factory->load_asset<yc::graphics::node>("Suzanne"));
    set_visible(true);
  }

  void update(yc::milliseconds delta) override
  {
    if (app->get_input()->key_pressed(yc::key::LEFT))
    {
      yaw += 0.3f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::RIGHT))
    {
      yaw -= 0.3f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::UP))
    {
      pitch += 0.3f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::DOWN))
    {
      pitch -= 0.3f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::PAGE_UP))
    {
      roll += 0.3f * delta;
      recalculate = true;
    }
    if (app->get_input()->key_pressed(yc::key::PAGE_DOWN))
    {
      roll -= 0.3f * delta;
      recalculate = true;
    }

    if (!recalculate)
      return;

    auto transform = glm::rotate(
          start_pos, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(
          transform, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(
          transform, glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));

    set_transform(transform);
    recalculate = false;
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}