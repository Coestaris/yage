/** ========================================================================
 *
 * @file mesh_buffer.hpp
 * @date 11-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <glm/common.hpp>

#include "GL/gl.h"
#include "glm/vec3.hpp"
#include "yagecore/app/asset.hpp"
#include "yagecore/graphics/graphics.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class material : public yc::app::asset
{
protected:
  std::shared_ptr<yc::graphics::graphics> graphics;

  float alpha_cutoff {};
  bool  double_sided {};

  struct
  {
    glm::vec3 base_color_factor;
    GLuint    base_color_texture;

    float  metallic_factor;
    float  roughness_factor;
    GLuint metallic_roughness_texture;

  } pbr {};

  GLuint normal_texture {};
  float  normal_scale {};

  GLuint occlusion_texture {};
  float  occlusion_strength {};

  glm::vec3 emissive_factor {};
  GLuint    emissive_texture {};

public:
  material(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics);

  yc::error load() override;
  void      unload() override;

  /* Better than nothing material */
  static std::shared_ptr<yc::graphics::material> basic_material();
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}