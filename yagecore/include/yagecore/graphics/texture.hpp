/** ========================================================================
 *
 * @file texture.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <utility>
#include <string>
#include <vector>

#include "yagecore/graphics/texture_param.hpp"
#include "yagecore/log.hpp"
#include "yagecore/app/asset.hpp"
#include "yagecore/util/uri.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class texture : public yc::app::asset
{
public:
  class data
  {
    std::vector<yc::util::uri> uris {};
    std::string                work_dir {};
    texture_param              parameters {};

  public:
    void add_uri(yc::util::uri uri);

    void set_parameters(const texture_param & parameters);
    void set_work_dir(const std::string & dir);

    bool operator==(const data & other) const;

    [[nodiscard]] std::vector<yc::util::uri> get_uris() const;
    [[nodiscard]] std::string                get_work_dir() const;
    [[nodiscard]] texture_param              get_parameters() const;

    bool empty() const;
  };

protected:
  /* little helper to load image,
   * so backends do not reinvent the wheel */
  static bool load_image(
        const yc::util::uri &  uri,
        std::vector<uint8_t> & out_data,
        size_t &               out_w,
        size_t &               out_h);

  std::shared_ptr<yc::graphics::graphics> graphics;
  data                                    sources;

  texture(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics);

public:
  void set_sources(const data & sources);
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}