// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "document.h"

#include <libpkgsource/recipe.h>

#include <string>
#include <vector>

namespace pkgsource::yaml::internal {

/// Decode package release identity fields from the validated package mapping.
[[nodiscard]] package_release
package_release_value(const node& package, const source_origin& origin);

/// Decode descriptive package metadata from the validated package mapping.
[[nodiscard]] package_metadata
package_metadata_value(const node& package, const source_origin& origin);

/// Decode ordered local and remote source inputs.
[[nodiscard]] std::vector<source_input> sources_value(
    const node& value, const source_origin& origin, const std::string& path);

/// Decode one exact POSIX-shell program mapping.
[[nodiscard]] program program_value(const node& value,
                                    const source_origin& origin,
                                    const std::string& path);

/// Decode all typed requirement scopes in protocol order.
[[nodiscard]] std::vector<requirement_declaration> requirements_value(
    const node& value, const source_origin& origin, const std::string& path);

/// Decode action-bound lifecycle programs.
[[nodiscard]] std::vector<lifecycle_program> lifecycle_value(
    const node& value, const source_origin& origin, const std::string& path);

/// Decode optional build and target architecture requirements.
[[nodiscard]] architecture_requirements
architectures_value(const node* value, const source_origin& origin);

} // namespace pkgsource::yaml::internal
