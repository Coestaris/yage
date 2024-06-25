/** ========================================================================
 *
 * @file asset_factory.hpp
 * @date 24-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <unordered_map>
#include <utility>
#include <functional>
#include "app/assets/shader.hpp"
#include "app/assets/asset_factory.hpp"
#include "app/assets/asset_cacher.hpp"
#include "json/json.h"
#include "core/util/named_thread.hpp"
#include "app/assets/texture.hpp"

namespace yage::app::assets::pack
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class asset_factory : public yage::app::assets::asset_factory
{
private:
  std::string extract_path {};
  std::string path;
  bool        destroyed = false;

  std::unordered_map<std::string, yage::app::assets::shader *>  shaders;
  std::unordered_map<std::string, yage::app::assets::texture *> textures;

  class pack_parser
  {
  private:
    using on_shader_callback  = std::function<void(
          const std::string &, const yage::app::assets::shader::sources &)>;
    using on_texture_callback = std::function<void(
          const std::string &,
          const yage::graphics::texture_param &,
          const std::vector<std::string> &)>;

    on_shader_callback  on_shader_cb;
    on_texture_callback on_texture_cb;

    yage::app::assets::shader::sources
    parse_shader_sources(const std::string & dir, const Json::Value & root);
    std::pair<yage::graphics::texture_param, std::vector<std::string>>
    parse_texture_sources(const std::string & dir, const Json::Value & root);

  public:
    pack_parser * on_shader(on_shader_callback cb)
    {
      on_shader_cb = std::move(cb);
      return this;
    }

    pack_parser * on_texture(on_texture_callback cb)
    {
      on_texture_cb = std::move(cb);
      return this;
    }

    yage::core::error
    parse(const std::string &      dir,
          std::vector<std::string> meta_files,
          bool                     fail_on_error = true);
  };

  class asset_cacher : public yage::app::assets::asset_cacher
  {
  private:
    std::string cache_path {};

  public:
    explicit asset_cacher(std::string cache_path)
    : cache_path(std::move(cache_path))
    {
    }

    yage::core::error cache_asset(
          const std::string & key, const std::vector<uint8_t> & data) override;
    yage::core::error get_cached_asset(
          const std::string & key, std::vector<uint8_t> & data) override;
  } cacher;

  class pack_watcher : public yage::core::util::named_thread
  {
  private:
    bool            should_stop = false;
    time_t          last_mtime  = 0;
    asset_factory * factory;

  public:
    explicit pack_watcher(asset_factory * factory)
    : named_thread("pack_watcher")
    , factory(factory)
    {
    }

    void run() override;
    void stop()
    {
      should_stop = true;
    }

  } watcher;

public:
  asset_factory(std::string path, std::string cache_path)
  : path(std::move(path))
  , cacher(std::move(cache_path))
  , watcher(this) {};

  ~asset_factory()
  {
    destroy();
  }

  yage::core::error read_assets(yage::graphics::graphics * graphics) override;
  yage::core::error destroy() override;

  yage::app::assets::shader *  get_shader(const std::string & name);
  yage::app::assets::texture * get_texture(const std::string & name);
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}