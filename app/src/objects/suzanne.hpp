/** ========================================================================
 *
 * @file suzanne.hpp
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
class suzanne : public yc::app::object
{
private:
  glm::mat4 start_pos = 0;

  float yaw {};
  float pitch {};
  float roll {};

  bool                              recalculate = true;
  std::shared_ptr<app::application> ctx;

public:
  ~        suzanne() override = default;
  explicit suzanne(std::shared_ptr<app::application> app);

  void
  on_ready(const std::shared_ptr<yc::app::asset_factory> & factory) override;
  void on_update(yc::milliseconds delta) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}