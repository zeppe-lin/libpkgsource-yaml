// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file parser.h
 *  \brief Strict native recipe and profile YAML parsing.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <libpkgsource/recipe.h>
#include <libpkgsource/snapshot.h>

namespace pkgsource::yaml {

/*! \brief Stable YAML frontend failure categories. */
enum class yaml_error_code {
  syntax,
  unsupported_feature,
  invalid_document,
  duplicate_key,
  unknown_key,
  missing_key,
  invalid_type,
  invalid_value,
  resource_limit,
};

/*! \brief Bounded parser resources for one in-memory document. */
struct parse_limits final {
  std::size_t maximum_document_bytes = 1024U * 1024U;
  std::size_t maximum_scalar_bytes = 256U * 1024U;
  std::size_t maximum_nodes = 65'536U;
  std::size_t maximum_depth = 64U;
};

/*! \brief Structured syntax error with exact diagnostic provenance. */
class yaml_error final : public std::runtime_error {
public:
  yaml_error(yaml_error_code code, std::string document, std::string path,
             std::uint32_t line, std::uint32_t column, std::string message);
  [[nodiscard]] yaml_error_code code() const noexcept;
  [[nodiscard]] const std::string& document() const noexcept;
  [[nodiscard]] const std::string& path() const noexcept;
  [[nodiscard]] std::uint32_t line() const noexcept;
  [[nodiscard]] std::uint32_t column() const noexcept;
private:
  yaml_error_code code_;
  std::string document_;
  std::string path_;
  std::uint32_t line_;
  std::uint32_t column_;
};

/*! \brief Parse one strict profiles document into parser-neutral declarations. */
[[nodiscard]] std::vector<profile_declaration> parse_profiles_yaml(
    std::string_view bytes, source_origin origin,
    const parse_limits& limits = {});

/*! \brief Parse one strict recipe document into one parser-neutral declaration. */
[[nodiscard]] recipe_declaration parse_recipe_yaml(
    std::string_view bytes, source_origin origin,
    const parse_limits& limits = {});

} // namespace pkgsource::yaml
