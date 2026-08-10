// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/test_support.h"

namespace {

using pkgsource::source_origin;
using pkgsource::yaml::parse_recipe_yaml;
using pkgsource::yaml::yaml_error_code;
using test_support::expect_yaml_error;

void rejects_unsupported_protocol_version()
{
  expect_yaml_error(yaml_error_code::invalid_value, "recipe.yml", "format", [] {
    (void)parse_recipe_yaml("format: zeppe-lin.recipe/2\n",
                            source_origin("recipe.yml"));
  });
}

void rejects_unknown_nested_keys()
{
  expect_yaml_error(
      yaml_error_code::unknown_key, "recipe.yml", "check.program", [] {
        (void)parse_recipe_yaml(
            R"(format: zeppe-lin.recipe/1
package:
  name: example
  version: 1
  release: 1
  summary: Example
  licenses: [MIT]
requirements: {}
sources: []
build: {language: posix-shell, script: echo}
check: {language: posix-shell, program: echo}
)",
            source_origin("recipe.yml"));
      });
}

void rejects_unknown_root_keys()
{
  expect_yaml_error(yaml_error_code::unknown_key, "recipe.yml", "unknown", [] {
    (void)parse_recipe_yaml("format: zeppe-lin.recipe/1\nunknown: true\n",
                            source_origin("recipe.yml"));
  });
}

void requires_package_section()
{
  expect_yaml_error(yaml_error_code::missing_key, "recipe.yml", "package", [] {
    (void)parse_recipe_yaml("format: zeppe-lin.recipe/1\n",
                            source_origin("recipe.yml"));
  });
}

void requires_canonical_release_integer()
{
  expect_yaml_error(
      yaml_error_code::invalid_value, "recipe.yml", "package.release", [] {
        (void)parse_recipe_yaml(
            "format: zeppe-lin.recipe/1\n"
            "package:\n"
            "  name: example\n"
            "  version: 1\n"
            "  release: 01\n"
            "  summary: Example\n"
            "  licenses: [MIT]\n"
            "requirements: {}\nsources: []\n"
            "build: {language: posix-shell, script: echo}\n",
            source_origin("recipe.yml"));
      });
}

void requires_explicit_requirement_subjects()
{
  expect_yaml_error(
      yaml_error_code::invalid_type, "recipe.yml", "requirements.build[0]", [] {
        (void)parse_recipe_yaml(
            "format: zeppe-lin.recipe/1\n"
            "package:\n"
            "  name: example\n"
            "  version: 1\n"
            "  release: 1\n"
            "  summary: Example\n"
            "  licenses: [MIT]\n"
            "requirements:\n"
            "  build: [gcc]\n"
            "sources: []\n"
            "build: {language: posix-shell, script: echo}\n",
            source_origin("recipe.yml"));
      });
}

void rejects_unknown_lifecycle_actions()
{
  expect_yaml_error(yaml_error_code::invalid_value,
                    "recipe.yml",
                    "requirements.lifecycle.configure",
                    [] {
                      (void)parse_recipe_yaml(
                          "format: zeppe-lin.recipe/1\n"
                          "package:\n"
                          "  name: example\n"
                          "  version: 1\n"
                          "  release: 1\n"
                          "  summary: Example\n"
                          "  licenses: [MIT]\n"
                          "requirements:\n"
                          "  lifecycle:\n"
                          "    configure: []\n"
                          "sources: []\n"
                          "build: {language: posix-shell, script: echo}\n",
                          source_origin("recipe.yml"));
                    });
}

void rejects_unknown_program_languages()
{
  expect_yaml_error(
      yaml_error_code::invalid_value, "recipe.yml", "build.language", [] {
        (void)parse_recipe_yaml("format: zeppe-lin.recipe/1\n"
                                "package:\n"
                                "  name: example\n"
                                "  version: 1\n"
                                "  release: 1\n"
                                "  summary: Example\n"
                                "  licenses: [MIT]\n"
                                "requirements: {}\nsources: []\n"
                                "build: {language: python, script: echo}\n",
                                source_origin("recipe.yml"));
      });
}

} // namespace

int main()
{
  return test_support::run({
      {"rejects unsupported protocol version",
       rejects_unsupported_protocol_version},
      {"rejects unknown nested keys", rejects_unknown_nested_keys},
      {"rejects unknown root keys", rejects_unknown_root_keys},
      {"requires package section", requires_package_section},
      {"requires canonical release integer",
       requires_canonical_release_integer},
      {"requires explicit requirement subjects",
       requires_explicit_requirement_subjects},
      {"rejects unknown lifecycle actions", rejects_unknown_lifecycle_actions},
      {"rejects unknown program languages", rejects_unknown_program_languages},
  });
}
