/** ========================================================================
 *
 * @file game_object.hpp
 * @date 19-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>

#include "glm/vec2.hpp"

#include "yagecore/time.hpp"
#include "yagecore/graphics/node.hpp"
#include "yagecore/app/asset_factory.hpp"

namespace yc::app
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class application;

namespace threads
{
class logic;
}

/** Base class for all objects in the application.
 * Object is a part of the application, that can be updated and rendered.
 * It can be a camera, a mesh, a light, etc.
 * User should inherit from this class to create custom objects. */
class object
{
private:
  yc::milliseconds last_update = 0;

protected:
  std::shared_ptr<yc::app::application> app;

public:
  struct renderer_data
  {
    /** If true, object should be rendered */
    bool visible {};
    /** If true, object is ready to be rendered (on_ready called) */
    bool ready {};
    /** Transformation matrix of the object */
    glm::mat4 transform = glm::mat4(1.0f);
    /** Node to render */
    std::shared_ptr<yc::graphics::node> node {};

  } render;

  class logic_data
  {
  private:
    bool should_be_deleted = false;

    friend class yc::app::threads::logic;
    friend class object;
    friend class application;

  } logic_data;

  virtual ~object() = default;
  explicit object(std::shared_ptr<yc::app::application> app)
  : app(std::move(app)) {};

protected:
  /** This function is called every frame, so it can be used
   * to update the object, process input, etc.
   *
   * Attention: This method is called from the logic thread. */
  virtual void update(yc::milliseconds delta) = 0;

public:
  /** This function is called when the object is ready to be rendered.
   * It may be used to load assets, create render passes, etc.
   * Note, that this may not be striclty synced with update calls
   * (e.g. when on_ready called after the first update).
   *
   * Attention: This method is called from the rendering thread. */
  virtual void on_ready(const std::shared_ptr<asset_factory> & factory) = 0;

  void update()
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

    update(delta);
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}