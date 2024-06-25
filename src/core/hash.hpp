/** ========================================================================
 *
 * @file hash.hpp
 * @date 25-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <vector>
#include <cstring>
#include <cstddef>
#include <cstdint>

namespace yage::core
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class hasher
{
public:
  hasher()          = default;
  virtual ~hasher() = default;

  virtual void add(const void * data, size_t size) = 0;
  virtual void add(const char * data)              = 0;
  virtual void add(const std::string & data)       = 0;

  virtual std::string          as_string() const = 0;
  virtual std::vector<uint8_t> as_bytes() const  = 0;
};

class fast_hasher : public hasher
{
private:
  std::vector<uint8_t> bytes;

public:
  fast_hasher()          = default;
  virtual ~fast_hasher() = default;

  void add(const void * data, size_t size) override
  {
    const uint8_t * b = static_cast<const uint8_t *>(data);
    this->bytes.insert(this->bytes.end(), b, b + size);
  }

  void add(const char * data) override
  {
    this->add(data, strlen(data));
  }

  void add(const std::string & data) override
  {
    this->add(data.c_str());
  }

  std::string as_string() const override
  {
    std::hash<std::string> hasher;
    return std::to_string(
          hasher(std::string(this->bytes.begin(), this->bytes.end())));
  }

  std::vector<uint8_t> as_bytes() const override
  {
    std::string hash = this->as_string();
    return std::vector<uint8_t>(hash.begin(), hash.end());
  }
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}