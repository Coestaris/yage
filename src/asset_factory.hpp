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

#include "json/json.h"
#include "core/util/named_thread.hpp"
#include "core/fs.hpp"

#include "graphics/texture_param.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

#include "app/asset_factory.hpp"
#include "graphics/asset_cacher.hpp"
#include "graphics/node.hpp"

namespace yageapp
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class asset_factory : public yage::app::asset_factory
{
private:
  std::string extract_path {};
  std::string path;
  bool        destroyed = false;

  std::unordered_map<std::string, yage::graphics::asset *> assets;

  class pack_parser
  {
  private:
    using on_shader_callback  = std::function<void(
          const std::string &, const yage::graphics::shader::data &)>;
    using on_texture_callback = std::function<void(
          const std::string &, const yage::graphics::texture::data &)>;
    using on_gltf_callback    = std::function<void(
          const std::string &, const yage::graphics::node::data &)>;

    on_shader_callback  on_shader_cb;
    on_texture_callback on_texture_cb;
    on_gltf_callback    on_gltf_cb;

    yage::graphics::shader::data
    parse_shader_sources(const std::string & dir, const Json::Value & root);
    yage::graphics::texture::data
    parse_texture_sources(const std::string & dir, const Json::Value & root);
    yage::graphics::node::data
    parse_gltf_sources(const std::string & dir, const Json::Value & root);

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

    pack_parser * on_gltf(on_gltf_callback cb)
    {
      on_gltf_cb = std::move(cb);
      return this;
    }

    yage::core::error
    parse(const std::string &      dir,
          std::vector<std::string> meta_files,
          bool                     fail_on_error = true);
  };

  class asset_cacher : public yage::graphics::asset_cacher
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

#if USE_DEBUG
  class pack_watchdog : public yage::core::util::named_thread
  {
  private:
    bool                     should_stop = false;
    yage::core::file_watcher file_watcher;
    asset_factory *          factory;

  public:
    explicit pack_watchdog(asset_factory * factory)
    : named_thread("pack_wdg")
    , file_watcher({ factory->path })
    , factory(factory)
    {
    }

    void run() override;
    void stop()
    {
      should_stop = true;
    }

  } watchdog;
#endif

public:
  asset_factory(std::string path, std::string cache_path)
  : path(std::move(path))
  , cacher(std::move(cache_path))
#if USE_DEBUG
  , watchdog(this)
#endif
          {};

  ~asset_factory()
  {
    destroy();
  }

  yage::core::error read_assets(yage::graphics::graphics * graphics) override;
  yage::core::error destroy() override;
  yage::graphics::asset * get_asset(const std::string & name) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}