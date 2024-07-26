/** ========================================================================
 *
 * @file data_source.hpp
 * @date 07-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <optional>

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
class data_source
{
  std::optional<std::string>            uri {};
  std::optional<std::string>            mime {};
  std::optional<std::vector<std::byte>> data {};

  static std::string mime_from_uri(const std::string & uri);

public:
  data_source() = default;
  data_source(std::string uri);
  data_source(const char * uri);
  data_source(const data_source & other);
  data_source(data_source && other);
  data_source(const std::vector<std::byte> & data, const std::string & mime);

  data_source & operator=(const data_source & other) = default;
  bool          operator==(const data_source & other) const;
  bool          operator!=(const data_source & other) const;

  [[nodiscard]] std::string get_mime() const;
  [[nodiscard]] std::string string() const;

  bool empty() const;

  void read(std::vector<std::byte> & data) const;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
}