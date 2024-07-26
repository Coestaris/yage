/** ========================================================================
 *
 * @file shader.hpp
 * @date 16-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <unordered_map>
#include <cstdint>
#include <utility>

#include "yagecore/app/asset.hpp"
#include "yagecore/log.hpp"
#include "yagecore/app/asset_cacher.hpp"
#include "yagecore/hash.hpp"
#include "yagecore/data_source.hpp"

namespace yc::graphics
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class graphics;

class shader : public yc::app::asset
{
public:
  class data
  {
    std::string language;
    std::string work_dir {};

    /* type -> source url */
    std::unordered_map<std::string, yc::data_source> data_sources;
    /* Cache for loaded sources. type -> content */
    std::unordered_map<std::string, std::string> content_cache;

  public:
    data() = default;
    data(std::string language);

    std::string get_hash(const std::string & name);

    void set_work_dir(const std::string & dir);
    void add_source(const std::string & type, const yc::data_source & src);

    bool operator==(data & other);

    [[nodiscard]] std::vector<std::string> get_types() const;
    [[nodiscard]] std::string              get_work_dir() const;
    [[nodiscard]] std::string              get_source(const std::string & type);
    [[nodiscard]] std::string              get_language() const;

    bool empty() const;
  };

protected:
  std::shared_ptr<yc::graphics::graphics> graphics;
  data                                    sources;

  shader(
        std::string                             name,
        std::shared_ptr<yc::app::asset_cacher>  cacher,
        std::shared_ptr<yc::graphics::graphics> graphics);

public:
  void set_sources(data & sources);
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}