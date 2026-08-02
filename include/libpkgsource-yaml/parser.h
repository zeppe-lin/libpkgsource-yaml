// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @file parser.h
 * @brief Strict, bounded YAML parsing into libpkgsource declarations.
 */
#pragma once

#include <libpkgsource-yaml/export.h>

#include <libpkgsource/recipe.h>
#include <libpkgsource/snapshot.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

/**
 * @namespace pkgsource::yaml
 * @brief Strict YAML syntax parsing into parser-neutral libpkgsource values.
 */
namespace pkgsource::yaml {

/** Stable parser failure categories. */
enum class yaml_error_code {
  /** The private YAML provider rejected malformed input. */
  syntax,
  /** The document uses YAML features excluded by the protocol subset. */
  unsupported_feature,
  /** The stream does not contain exactly one non-empty document. */
  invalid_document,
  /** A mapping repeats a key. */
  duplicate_key,
  /** A mapping contains a key not admitted by the document protocol. */
  unknown_key,
  /** A required protocol key is absent. */
  missing_key,
  /** A node has the wrong scalar, sequence, or mapping kind. */
  invalid_type,
  /** A scalar or declaration value violates its protocol contract. */
  invalid_value,
  /** Parsing exceeded a caller-selected resource ceiling. */
  resource_limit,
};

/**
 * Bounded resources for one caller-owned in-memory document.
 *
 * All limits are inclusive. A value equal to a configured ceiling is accepted;
 * the next byte, scalar byte, node, or nesting level is rejected.
 */
struct parse_limits final {
  /** Maximum total input bytes presented to the YAML provider. */
  std::size_t maximum_document_bytes = 1024U * 1024U;
  /** Maximum bytes retained from one scalar event. */
  std::size_t maximum_scalar_bytes = 256U * 1024U;
  /** Maximum scalar, sequence, and mapping nodes in the document tree. */
  std::size_t maximum_nodes = 65'536U;
  /** Maximum node depth, counting the document root as depth one. */
  std::size_t maximum_depth = 64U;
};

/**
 * Structured parser failure with stable category and source provenance.
 *
 * The human-readable `what()` string is diagnostic text, not a stable machine
 * interface. Callers should branch on code() and use document(), path(),
 * line(), and column() for reporting.
 */
class PKGSOURCE_YAML_API yaml_error final : public std::runtime_error {
public:
  /**
   * Construct one parser failure.
   *
   * @param code Stable failure category.
   * @param document Caller-supplied document label.
   * @param path Protocol path associated with the failure.
   * @param line One-based source line, or zero when unavailable.
   * @param column One-based source column, or zero when unavailable.
   * @param message Human-readable diagnostic message.
   */
  yaml_error(yaml_error_code code,
             std::string document,
             std::string path,
             std::uint32_t line,
             std::uint32_t column,
             std::string message);
  /** Destroy one parser failure. */
  ~yaml_error() override;

  /** Return the stable parser failure category. */
  [[nodiscard]] yaml_error_code code() const noexcept;
  /** Return the caller-supplied document label. */
  [[nodiscard]] const std::string& document() const noexcept;
  /** Return the protocol path associated with the failure. */
  [[nodiscard]] const std::string& path() const noexcept;
  /** Return the one-based source line. */
  [[nodiscard]] std::uint32_t line() const noexcept;
  /** Return the one-based source column. */
  [[nodiscard]] std::uint32_t column() const noexcept;

private:
  yaml_error_code code_;
  std::string document_;
  std::string path_;
  std::uint32_t line_;
  std::uint32_t column_;
};

/**
 * Parse one `zeppe-lin.profiles/1` document.
 *
 * @param bytes Caller-owned document bytes. The parser does not retain the
 *     string view after return.
 * @param origin Diagnostic provenance attached to returned declarations.
 * @param limits Inclusive parser resource ceilings.
 * @return Parser-neutral profile declarations in document order.
 * @throws yaml_error on syntax, protocol, value, or resource failure.
 * @throws std::bad_alloc if allocation fails.
 *
 * This function does not seal a profile catalog. The caller may aggregate
 * declarations from multiple explicit documents before invoking
 * `pkgsource::profile_catalog::seal()`.
 */
[[nodiscard]] PKGSOURCE_YAML_API std::vector<profile_declaration>
parse_profiles_yaml(std::string_view bytes,
                    source_origin origin,
                    const parse_limits& limits = {});

/**
 * Parse one `zeppe-lin.recipe/1` document.
 *
 * @param bytes Caller-owned document bytes. The parser does not retain the
 *     string view after return.
 * @param origin Diagnostic provenance attached to the returned declaration.
 * @param limits Inclusive parser resource ceilings.
 * @return One parser-neutral recipe declaration.
 * @throws yaml_error on syntax, protocol, value, or resource failure.
 * @throws std::bad_alloc if allocation fails.
 *
 * This function does not seal source authority. The caller must supply the
 * resulting declaration and the selected profile catalog to
 * `pkgsource::seal_source()`.
 */
[[nodiscard]] PKGSOURCE_YAML_API recipe_declaration
parse_recipe_yaml(std::string_view bytes,
                  source_origin origin,
                  const parse_limits& limits = {});

} // namespace pkgsource::yaml
