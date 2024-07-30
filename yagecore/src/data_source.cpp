/** ========================================================================
 *
 * @file data_source.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/data_source.hpp"

#include <algorithm>
#include <fstream>
#include <regex>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
static const char * default_mime = "text/plain";

std::string yc::data_source::mime_from_uri(const std::string & uri)
{
  size_t pos = uri.find_last_of('.');
  if (pos == std::string::npos)
  {
    return "unk";
  }

  /* for now just return extension */
  std::string ext = uri.substr(pos + 1);
  if (ext == "jpeg" || ext == "jpg")
    return "image/jpeg";
  if (ext == "png")
    return "image/png";
  if (ext == "gif")
    return "image/gif";

  return default_mime;
}

/* Shared functions ========================================================= */
yc::data_source::data_source(std::string uri)
: uri(std::move(uri))
, mime(mime_from_uri(uri)) {};
yc::data_source::data_source(const char * uri)
: uri(uri)
, mime(mime_from_uri(uri)) {};
yc::data_source::data_source(const data_source & other)
: uri(other.uri)
, mime(other.mime)
, data(other.data) {};
yc::data_source::data_source(data_source && other)
: uri(std::move(other.uri))
, mime(std::move(other.mime))
, data(std::move(other.data)) {};
yc::data_source::data_source(
      const std::vector<std::byte> & data, const std::string & mime)
: mime(mime)
, data(data) {};

bool yc::data_source::operator==(const data_source & other) const
{
  return uri == other.uri && mime == other.mime && data == other.data;
}

bool yc::data_source::operator!=(const data_source & other) const
{
  return !(*this == other);
}

bool yc::data_source::empty() const
{
  if (uri)
  {
    return uri->empty();
  }

  if (data)
  {
    return data->empty();
  }

  return true;
}

yc::error read(std::string & data) const
{

}

yc::error yc::data_source::read(std::vector<std::byte> & data) const
{
  data.clear();

  if (this->data)
  {
    data = *this->data;
  }

  if (uri)
  {
    std::ifstream file(uri.value(), std::ios::binary);
    if (!file.is_open())
    {
      return yc::error::FILE_NOT_FOUND;
    }

    file.seekg(0, std::ios::end);
    data.resize(file.tellg());
    file.seekg(0, std::ios::beg);

    file.read(reinterpret_cast<char *>(data.data()), data.size());

    file.close();
  }


}

std::string yc::data_source::get_mime() const
{
  if (mime)
  {
    return *mime;
  }

  return default_mime;
}

std::string yc::data_source::string() const
{
  if (uri)
  {
    return *uri;
  }

  if (data)
  {
    return "data(" + std::to_string(data->size()) + ")";
  }

  return "empty";
}