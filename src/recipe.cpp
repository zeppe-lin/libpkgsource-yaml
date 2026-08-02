// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include "internal/document.h"
#include "internal/recipe_fields.h"

#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace pkgsource::yaml {

recipe_declaration parse_recipe_yaml(std::string_view bytes,
                                     source_origin origin,
                                     const parse_limits& limits)
{
  using namespace internal;

  node root = parse_document(bytes, origin, limits);
  allow_keys(root, origin, "$",
             {"format", "package", "requirements", "sources", "build",
              "check", "lifecycle", "architectures"});
  require_format(root, origin, "zeppe-lin.recipe/1");

  const node& package = required_key(root, "package", origin, "$");
  require_kind(package, node_kind::mapping, origin, "package", "package");

  const node& requirements = required_key(root, "requirements", origin, "$");
  const node& sources = required_key(root, "sources", origin, "$");
  const node& build = required_key(root, "build", origin, "$");

  std::optional<program> check;
  if (const node* value = find_key(root, "check")) {
    check = program_value(*value, origin, "check");
  }

  std::vector<lifecycle_program> lifecycle;
  if (const node* value = find_key(root, "lifecycle")) {
    lifecycle = lifecycle_value(*value, origin, "lifecycle");
  }

  return recipe_declaration(
      package_release_value(package, origin),
      package_metadata_value(package, origin),
      sources_value(sources, origin, "sources"),
      program_value(build, origin, "build"),
      requirements_value(requirements, origin, "requirements"),
      std::move(lifecycle),
      architectures_value(find_key(root, "architectures"), origin),
      provenance(origin, "document", root), std::move(check));
}

} // namespace pkgsource::yaml
