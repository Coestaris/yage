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
#include "yagecore/named_thread.hpp"
#include "yagecore/fs.hpp"

#include "yagecore/graphics/shader.hpp"
#include "yagecore/graphics/texture.hpp"
#include "yagecore/graphics/node.hpp"
#include "yagecore/graphics/font.hpp"

#include "yagecore/app/asset_factory.hpp"
#include "yagecore/app/asset_cacher.hpp"

namespace yagepack
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class asset_factory : public yc::app::asset_factory
{
private:
  std::string extract_path {};
  std::string path;
  std::string cache_path;

  std::unordered_map<std::string, std::shared_ptr<yc::app::asset>> assets;

  class pack_parser
  {
  private:
    using on_shader_callback  = std::function<void(
          const std::string &, const yc::graphics::shader::data &)>;
    using on_texture_callback = std::function<void(
          const std::string &, const yc::graphics::texture::data &)>;
    using on_gltf_callback    = std::function<void(
          const std::string &, const yc::graphics::node::data &)>;
    using on_font_callback    = std::function<void(
          const std::string &, const yc::graphics::font::data &)>;

    on_shader_callback  on_shader_cb;
    on_texture_callback on_texture_cb;
    on_gltf_callback    on_gltf_cb;
    on_font_callback    on_font_cb;

    yc::graphics::shader::data
    parse_shader_sources(const std::string & dir, const Json::Value & root);
    yc::graphics::texture::data
    parse_texture_sources(const std::string & dir, const Json::Value & root);
    yc::graphics::node::data
    parse_gltf_sources(const std::string & dir, const Json::Value & root);
    yc::graphics::font::data
    parse_font_sources(const std::string & dir, const Json::Value & root);

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

    pack_parser * on_font(on_font_callback cb)
    {
      on_font_cb = std::move(cb);
      return this;
    }

    yc::error
    parse(const std::string &      dir,
          std::vector<std::string> meta_files,
          bool                     fail_on_error = true);
  };

  class asset_cacher : public yc::app::asset_cacher
  {
  private:
    std::string cache_path {};

  public:
    ~asset_cacher() override = default;

    explicit asset_cacher(std::string cache_path)
    : cache_path(std::move(cache_path))
    {
    }

    yc::error cache_asset(
          const std::string & key, const std::vector<uint8_t> & data) override;
    yc::error get_cached_asset(
          const std::string & key, std::vector<uint8_t> & data) override;
  };

#if USE_DEBUG
  class pack_watchdog : public yc::named_thread
  {
  private:
    bool             should_stop = false;
    yc::file_watcher file_watcher;
    asset_factory *  factory;

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
  , cache_path(std::move(cache_path))
#if USE_DEBUG
  , watchdog(this)
#endif
          {};

  ~asset_factory()
  {
    destroy();
  }

  yc::error
  read_assets(std::shared_ptr<yc::graphics::graphics> graphics) override;
  yc::error                       destroy() override;
  std::shared_ptr<yc::app::asset> get_asset(const std::string & name) override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}