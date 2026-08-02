// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/documents.h"
#include "../support/test_support.h"

#include <libpkgsource/error.h>
#include <libpkgsource/profile.h>
#include <libpkgsource/snapshot.h>

namespace {

pkgsource::profile_catalog profiles()
{
  return pkgsource::profile_catalog::seal(pkgsource::yaml::parse_profiles_yaml(
      test_documents::profiles, pkgsource::source_origin("profiles.yml")));
}

pkgsource::source_snapshot parse_and_seal(std::string_view document)
{
  const pkgsource::profile_catalog catalog = profiles();
  return pkgsource::seal_source(
      pkgsource::source_origin("recipe.yml"),
      pkgsource::yaml::parse_recipe_yaml(
          document, pkgsource::source_origin("recipe.yml")),
      catalog);
}

void lifecycle_requirements_require_matching_programs()
{
  test_support::expect_core_error(pkgsource::error_code::invalid_recipe, [] {
    (void)parse_and_seal(R"(format: zeppe-lin.recipe/1
package:
  name: example
  version: 1
  release: 1
  summary: Example
  licenses: [MIT]
requirements:
  lifecycle:
    post-install:
      - package: desktop-file-utils
sources: []
build: {language: posix-shell, script: echo}
)");
  });
}

void check_requirements_require_a_check_program()
{
  test_support::expect_core_error(pkgsource::error_code::invalid_recipe, [] {
    (void)parse_and_seal(R"(format: zeppe-lin.recipe/1
package:
  name: example
  version: 1
  release: 1
  summary: Example
  licenses: [MIT]
requirements:
  check:
    - package: pkgcheck
sources: []
build: {language: posix-shell, script: "true\n"}
)");
  });
}

void duplicate_source_names_remain_a_sealer_error()
{
  test_support::expect_core_error(pkgsource::error_code::duplicate_declaration,
                                  [] {
                                    (void)parse_and_seal(
                                        R"(format: zeppe-lin.recipe/1
package:
  name: example
  version: 1
  release: 1
  summary: Example
  licenses: [MIT]
requirements: {}
sources:
  - {url: https://example.invalid/a, name: source.tar, sha256: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef}
  - {url: https://example.invalid/b, name: source.tar, sha256: abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789}
build: {language: posix-shell, script: echo}
)");
                                  });
}

} // namespace

int main()
{
  return test_support::run({
      {"lifecycle requirements require matching programs",
       lifecycle_requirements_require_matching_programs},
      {"check requirements require a check program",
       check_requirements_require_a_check_program},
      {"duplicate source names remain a sealer error",
       duplicate_source_names_remain_a_sealer_error},
  });
}
