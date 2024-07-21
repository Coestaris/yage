/** ========================================================================
 *
 * @file uri.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/util/uri.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
yc::util::uri::uri(std::string uri)
: uri_(std::move(uri)) {};
yc::util::uri::uri(const char * uri)
: uri_(uri) {};
yc::util::uri::uri(const uri & other)
: uri_(other.uri_) {};
yc::util::uri::uri(uri && other)
: uri_(std::move(other.uri_)) {};

yc::util::uri & yc::util::uri::operator=(const uri & other)
{
  uri_ = other.uri_;
  return *this;
}

bool yc::util::uri::operator==(const uri & other) const
{
  return uri_ == other.uri_;
}

bool yc::util::uri::operator!=(const uri & other) const
{
  return uri_ != other.uri_;
}

std::string yc::util::uri::get_uri() const
{
  return uri_;
}

bool yc::util::uri::empty() const
{
  return uri_.empty();
}

void yc::util::uri::read(std::vector<uint8_t> & data) const
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

std::string yc::util::uri::get_mime() const
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