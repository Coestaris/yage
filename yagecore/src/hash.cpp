/** ========================================================================
 *
 * @file hash.cpp
 * @date 21-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net.
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "yagecore/hash.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::fast_hasher::add(const void * data, size_t size)
{
  const uint8_t * b = static_cast<const uint8_t *>(data);
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
  return std::to_string(
        hasher(std::string(this->bytes.begin(), this->bytes.end())));
}

std::vector<uint8_t> yc::fast_hasher::as_bytes() const
{
  std::string hash = this->as_string();
  return std::vector<uint8_t>(hash.begin(), hash.end());
}