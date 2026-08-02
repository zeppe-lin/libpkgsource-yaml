// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <libpkgsource-yaml/parser.h>

#include "source_mark.h"

#include <libpkgsource/error.h>

#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

namespace pkgsource::yaml::internal {

/// Provider-neutral node kinds admitted by the strict document subset.
enum class node_kind {
  scalar,
  sequence,
  mapping
};

/// One bounded document-tree node retaining exact scalar bytes and source mark.
struct node final {
  node_kind kind;
  source_mark mark;
  std::string scalar;
  std::vector<node> children;
};

/// Throw a structured parser failure at one normalized source location.
[[noreturn]] void fail(yaml_error_code code,
                       const source_origin& origin,
                       std::string_view path,
                       source_mark mark,
                       std::string message);

/** Parse and validate exactly one bounded strict-subset YAML document. */
[[nodiscard]] node parse_document(std::string_view bytes,
                                  const source_origin& origin,
                                  const parse_limits& limits);

/// Append one mapping key to a diagnostic protocol path.
[[nodiscard]] std::string child_path(std::string_view path,
                                     std::string_view key);

/// Require one node kind and return the original node for fluent validation.
const node& require_kind(const node& value,
                         node_kind expected,
                         const source_origin& origin,
                         std::string_view path,
                         std::string_view name);

/// Find one already-validated scalar mapping key without allocating.
[[nodiscard]] const node* find_key(const node& mapping, std::string_view key);

/// Return one required mapping value or throw missing_key at its protocol path.
[[nodiscard]] const node& required_key(const node& mapping,
                                       std::string_view key,
                                       const source_origin& origin,
                                       std::string_view path);

/// Reject every mapping key outside the supplied exact schema set.
void allow_keys(const node& mapping,
                const source_origin& origin,
                std::string_view path,
                std::initializer_list<std::string_view> allowed);

/// Return exact scalar bytes after enforcing the expected node kind.
[[nodiscard]] const std::string& scalar_value(const node& value,
                                              const source_origin& origin,
                                              std::string_view path);

/// Return sequence children after enforcing the expected node kind.
[[nodiscard]] const std::vector<node>& sequence_value(
    const node& value, const source_origin& origin, std::string_view path);

/// Construct declaration provenance from one normalized tree location.
[[nodiscard]] declaration_provenance
provenance(const source_origin& origin, std::string path, const node& value);

/// Decode one explicit package-or-profile requirement subject mapping.
[[nodiscard]] requirement_subject subject_value(const node& value,
                                                const source_origin& origin,
                                                const std::string& path);

/// Require the exact document protocol format scalar.
void require_format(const node& root,
                    const source_origin& origin,
                    std::string_view expected);

/**
 * Construct one libpkgsource value and translate only owner validation errors.
 *
 * The supplied function executes synchronously. Allocation, logic, and other
 * unrelated exceptions retain their original types.
 */
template <typename Function>
auto semantic_value(const source_origin& origin,
                    const std::string& path,
                    const node& value,
                    Function&& function) -> decltype(function())
{
  try {
    return function();
  } catch (const pkgsource::error& failure) {
    fail(yaml_error_code::invalid_value,
         origin,
         path,
         value.mark,
         failure.what());
  }
}

} // namespace pkgsource::yaml::internal
