/** ========================================================================
 *
 * @file uri.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <cstdint>

namespace yc::util
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */

class uri
{
private:
  std::string uri_;

public:
  uri() = default;
  uri(std::string uri)
  : uri_(std::move(uri)) {};
  uri(const char * uri)
  : uri_(uri) {};
  uri(const uri & other)
  : uri_(other.uri_) {};
  uri(uri && other) noexcept
  : uri_(std::move(other.uri_)) {};

  uri & operator=(const uri & other)
  {
    uri_ = other.uri_;
    return *this;
  }

  bool operator==(const uri & other) const
  {
    return uri_ == other.uri_;
  }

  bool operator!=(const uri & other) const
  {
    return uri_ != other.uri_;
  }

  std::string get_uri() const
  {
    return uri_;
  }

  bool empty() const
  {
    return uri_.empty();
  }

  void read(std::vector<uint8_t> & data) const
  {
    data.clear();

    /* Handle only file scheme for now */
    std::ifstream file(uri_, std::ios::binary);
    if (!file.is_open())
    {
      return;
    }

    file.seekg(0, std::ios::end);
    data.resize(file.tellg());
    file.seekg(0, std::ios::beg);

    file.read(reinterpret_cast<char *>(data.data()), data.size());

    file.close();
  }

  std::string get_mime() const
  {
    if (uri_.empty())
    {
      return "unk";
    }

    size_t pos = uri_.find_last_of('.');
    if (pos == std::string::npos)
    {
      return "unk";
    }

    /* for now just return extension */
    std::string ext = uri_.substr(pos + 1);
    if (ext == "jpeg" || ext == "jpg")
      return "image/jpeg";
    if (ext == "png")
      return "image/png";
    if (ext == "gif")
      return "image/gif";

    return "unk";
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}