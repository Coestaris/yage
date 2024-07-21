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
  uri(std::string uri);
  uri(const char * uri);
  uri(const uri & other);
  uri(uri && other);

  uri & operator=(const uri & other);
  bool  operator==(const uri & other) const;
  bool  operator!=(const uri & other) const;

  [[nodiscard]] std::string get_uri() const;
  [[nodiscard]] std::string get_mime() const;

  bool empty() const;

  void read(std::vector<uint8_t> & data) const;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}