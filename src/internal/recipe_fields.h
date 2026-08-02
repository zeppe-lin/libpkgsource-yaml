// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "document.h"

#include <libpkgsource/recipe.h>

#include <optional>
#include <string>
#include <vector>

namespace pkgsource::yaml::internal {

[[nodiscard]] package_release package_release_value(
    const node& package, const source_origin& origin);
[[nodiscard]] package_metadata package_metadata_value(
    const node& package, const source_origin& origin);
[[nodiscard]] std::vector<source_input> sources_value(
    const node& value, const source_origin& origin, const std::string& path);
[[nodiscard]] program program_value(const node& value,
                                    const source_origin& origin,
                                    const std::string& path);
[[nodiscard]] std::vector<requirement_declaration> requirements_value(
    const node& value, const source_origin& origin, const std::string& path);
[[nodiscard]] std::vector<lifecycle_program> lifecycle_value(
    const node& value, const source_origin& origin, const std::string& path);
[[nodiscard]] architecture_requirements architectures_value(
    const node* value, const source_origin& origin);

} // namespace pkgsource::yaml::internal
