/** ========================================================================
 *
 * @file material.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/graphics/material.hpp"
#include "glm/vec3.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::graphics::material::material(
      std::string                             name,
      std::shared_ptr<yc::app::asset_cacher>  cacher,
      std::shared_ptr<yc::graphics::graphics> graphics)
: yc::app::asset(std::move(name), std::move(cacher))
, graphics(std::move(graphics)) {};

yc::error yc::graphics::material::load()
{
  return OK;
}
void yc::graphics::material::unload()
{
}

/* Better than nothing material */
std::shared_ptr<yc::graphics::material> yc::graphics::material::basic_material()
{
  auto mat
        = std::make_shared<yc::graphics::material>("default", nullptr, nullptr);
  mat->pbr.base_color_factor = glm::vec3(1.0f);
  mat->pbr.metallic_factor   = 0.0f;
  mat->pbr.roughness_factor  = 1.0f;
  mat->emissive_factor       = glm::vec3(0.0f);
  mat->alpha_cutoff          = 0.5f;

  return mat;
}