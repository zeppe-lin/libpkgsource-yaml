// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/test_support.h"

#include <string>

namespace {

using pkgsource::source_origin;
using pkgsource::yaml::parse_profiles_yaml;
using pkgsource::yaml::yaml_error_code;
using test_support::expect_yaml_error;

void rejects_duplicate_mapping_keys()
{
  expect_yaml_error(
      yaml_error_code::duplicate_key, "profiles.yml", "format", [] {
        (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                  "format: zeppe-lin.profiles/1\n"
                                  "profiles: {}\n",
                                  source_origin("profiles.yml"));
      });
}

void rejects_unknown_schema_keys()
{
  expect_yaml_error(yaml_error_code::unknown_key, "profiles.yml", "extra", [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\nprofiles: {}\nextra: no\n",
        source_origin("profiles.yml"));
  });
}

void rejects_anchors()
{
  expect_yaml_error(
      yaml_error_code::unsupported_feature, "profiles.yml", "$", [] {
        (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                  "profiles:\n"
                                  "  compiler: &compiler\n"
                                  "    members:\n"
                                  "      - package: gcc\n",
                                  source_origin("profiles.yml"));
      });
}

void rejects_aliases()
{
  expect_yaml_error(
      yaml_error_code::unsupported_feature, "profiles.yml", "$", [] {
        (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                  "profiles: *compiler\n",
                                  source_origin("profiles.yml"));
      });
}

void rejects_directives()
{
  expect_yaml_error(
      yaml_error_code::unsupported_feature, "profiles.yml", "$", [] {
        (void)parse_profiles_yaml(
            "%YAML 1.2\n---\n"
            "format: zeppe-lin.profiles/1\nprofiles: {}\n",
            source_origin("profiles.yml"));
      });
}

void rejects_merge_keys()
{
  expect_yaml_error(yaml_error_code::unsupported_feature,
                    "profiles.yml",
                    "profiles.compiler",
                    [] {
                      (void)parse_profiles_yaml(
                          "format: zeppe-lin.profiles/1\n"
                          "profiles:\n"
                          "  compiler:\n"
                          "    <<: {members: [{package: gcc}]}\n",
                          source_origin("profiles.yml"));
                    });
}

void rejects_custom_tags()
{
  expect_yaml_error(
      yaml_error_code::unsupported_feature, "profiles.yml", "$", [] {
        (void)parse_profiles_yaml("format: zeppe-lin.profiles/1\n"
                                  "profiles: !zeppe/profiles {}\n",
                                  source_origin("profiles.yml"));
      });
}

void rejects_multiple_documents()
{
  expect_yaml_error(yaml_error_code::invalid_document, "profiles.yml", "$", [] {
    (void)parse_profiles_yaml(
        "---\nformat: zeppe-lin.profiles/1\nprofiles: {}\n"
        "---\nformat: zeppe-lin.profiles/1\nprofiles: {}\n",
        source_origin("profiles.yml"));
  });
}

void rejects_complex_mapping_keys()
{
  expect_yaml_error(yaml_error_code::invalid_type, "profiles.yml", "$", [] {
    (void)parse_profiles_yaml(
        "? [format]\n: zeppe-lin.profiles/1\nprofiles: {}\n",
        source_origin("profiles.yml"));
  });
}

void rejects_empty_documents()
{
  expect_yaml_error(yaml_error_code::invalid_document, "profiles.yml", "$", [] {
    (void)parse_profiles_yaml("", source_origin("profiles.yml"));
  });
}

void reports_provider_syntax_failures()
{
  expect_yaml_error(yaml_error_code::syntax, "profiles.yml", "$", [] {
    const std::string invalid_utf8("\xff", 1);
    (void)parse_profiles_yaml(invalid_utf8, source_origin("profiles.yml"));
  });
}

} // namespace

int main()
{
  return test_support::run({
      {"rejects duplicate mapping keys", rejects_duplicate_mapping_keys},
      {"rejects unknown schema keys", rejects_unknown_schema_keys},
      {"rejects anchors", rejects_anchors},
      {"rejects aliases", rejects_aliases},
      {"rejects directives", rejects_directives},
      {"rejects merge keys", rejects_merge_keys},
      {"rejects custom tags", rejects_custom_tags},
      {"rejects multiple documents", rejects_multiple_documents},
      {"rejects complex mapping keys", rejects_complex_mapping_keys},
      {"rejects empty documents", rejects_empty_documents},
      {"reports provider syntax failures", reports_provider_syntax_failures},
  });
}
