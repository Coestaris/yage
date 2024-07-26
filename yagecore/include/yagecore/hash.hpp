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

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class hasher
{
public:
           hasher() = default;
  virtual ~hasher() = default;

  virtual void add(const void * data, size_t size) = 0;
  virtual void add(const char * data)              = 0;
  virtual void add(const std::string & data)       = 0;

  virtual std::string            as_string() const = 0;
  virtual std::vector<std::byte> as_bytes() const  = 0;
};

class fast_hasher : public hasher
{
  std::vector<std::byte> bytes;

public:
           fast_hasher() = default;
  virtual ~fast_hasher() = default;

  void add(const void * data, size_t size) override;
  void add(const char * data) override;
  void add(const std::string & data) override;
  void add(int data);

  std::string            as_string() const override;
  std::vector<std::byte> as_bytes() const override;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}