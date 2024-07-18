/** ========================================================================
 *
 * @file app.hpp
 * @date 18-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <yagecore/app/application.hpp>

namespace app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class application : public yc::app::application
{
  static std::string lookup_asset_path();

public:
  glm::mat4 view_matrix;
  glm::mat4 proj_matrix;

  ~application() override = default;
   application(
         std::shared_ptr<yc::app::asset_factory>       asset_factory,
         const std::shared_ptr<yc::graphics::window> & window,
         std::shared_ptr<yc::os_events>                os_events)
  : yc::app::application(
          std::move(asset_factory), window, std::move(os_events)) {};

  std::vector<std::shared_ptr<yc::graphics::render_pass>> create_render_chain(
        const std::shared_ptr<yc::graphics::graphics> & graphics,
        const std::shared_ptr<yc::app::asset_factory> & asset_factory) override;

  static int run();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}