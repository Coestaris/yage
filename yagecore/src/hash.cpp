/** ========================================================================
 *
 * @file hash.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net.
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/hash.hpp"

#include <algorithm>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::fast_hasher::add(const void * data, size_t size)
{
  const auto * b = static_cast<const std::byte *>(data);
  this->bytes.insert(this->bytes.end(), b, b + size);
}

void yc::fast_hasher::add(const char * data)
{
  this->add(data, strlen(data));
}

void yc::fast_hasher::add(const std::string & data)
{
  this->add(data.c_str());
}

void yc::fast_hasher::add(int data)
{
  this->add(&data, sizeof(data));
}

std::string yc::fast_hasher::as_string() const
{
  std::hash<std::string> hasher;
  std::string            result = std::to_string(hasher(std::string(
        reinterpret_cast<const char *>(this->bytes.data()),
        this->bytes.size())));

  return result;
}

std::vector<std::byte> yc::fast_hasher::as_bytes() const
{
  std::string            hash = this->as_string();
  std::vector<std::byte> result(hash.size());
  std::transform(hash.begin(), hash.end(), result.begin(), [](char c) {
    return static_cast<std::byte>(c);
  });

  return result;
}