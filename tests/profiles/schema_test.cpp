// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/test_support.h"

namespace {

using pkgsource::source_origin;
using pkgsource::yaml::parse_profiles_yaml;
using pkgsource::yaml::yaml_error_code;
using test_support::expect_yaml_error;

void rejects_unsupported_protocol_version()
{
  expect_yaml_error(
      yaml_error_code::invalid_value, "profiles.yml", "format", [] {
        (void)parse_profiles_yaml(
            "format: zeppe-lin.profiles/2\nprofiles: {}\n",
            source_origin("profiles.yml"));
      });
}

void requires_profiles_mapping()
{
  expect_yaml_error(
      yaml_error_code::invalid_type, "profiles.yml", "profiles", [] {
        (void)parse_profiles_yaml(
            "format: zeppe-lin.profiles/1\nprofiles: []\n",
            source_origin("profiles.yml"));
      });
}

void reports_invalid_profile_names_at_their_schema_path()
{
  expect_yaml_error(
      yaml_error_code::invalid_value, "profiles.yml", "profiles.Toolchain", [] {
        (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                  "profiles:\n"
                                  "  Toolchain:\n"
                                  "    members:\n"
                                  "      - package: gcc\n",
                                  source_origin("profiles.yml"));
      });
}

void rejects_profiles_without_members()
{
  expect_yaml_error(
      yaml_error_code::invalid_value, "profiles.yml", "profiles.empty", [] {
        (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                  "profiles:\n"
                                  "  empty:\n"
                                  "    members: []\n",
                                  source_origin("profiles.yml"));
      });
}

void rejects_scalar_member_shorthand()
{
  expect_yaml_error(yaml_error_code::invalid_type,
                    "profiles.yml",
                    "profiles.compiler.members[0]",
                    [] {
                      (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                                "profiles:\n"
                                                "  compiler:\n"
                                                "    members:\n"
                                                "      - gcc\n",
                                                source_origin("profiles.yml"));
                    });
}

} // namespace

int main()
{
  return test_support::run({
      {"rejects unsupported protocol version",
       rejects_unsupported_protocol_version},
      {"requires profiles mapping", requires_profiles_mapping},
      {"reports invalid profile names at their path",
       reports_invalid_profile_names_at_their_schema_path},
      {"rejects profiles without members", rejects_profiles_without_members},
      {"rejects scalar member shorthand", rejects_scalar_member_shorthand},
  });
}
