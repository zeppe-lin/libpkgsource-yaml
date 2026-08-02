// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/test_support.h"

#include <cstddef>
#include <string>

namespace {

constexpr std::string_view document =
    "format: zeppe-lin.profiles/1\n"
    "profiles:\n"
    "  compiler:\n"
    "    members:\n"
    "      - package: gcc\n";

void expect_limit(const pkgsource::yaml::parse_limits& limits)
{
  test_support::expect_yaml_error(
      pkgsource::yaml::yaml_error_code::resource_limit, "profiles.yml", "",
      [&] {
        (void)pkgsource::yaml::parse_profiles_yaml(
            document, pkgsource::source_origin("profiles.yml"), limits);
      });
}

void rejects_each_resource_beyond_its_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_document_bytes = document.size() - 1;
  expect_limit(limits);

  limits = {};
  limits.maximum_scalar_bytes = 8;
  expect_limit(limits);

  limits = {};
  limits.maximum_nodes = 4;
  expect_limit(limits);

  limits = {};
  limits.maximum_depth = 3;
  expect_limit(limits);
}

void accepts_each_exact_boundary()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_document_bytes = document.size();
  test_support::require_equal(
      pkgsource::yaml::parse_profiles_yaml(
          document, pkgsource::source_origin("profiles.yml"), limits)
          .size(),
      std::size_t{1}, "exact document-byte boundary must be accepted");

  limits = {};
  limits.maximum_scalar_bytes = std::string("zeppe-lin.profiles/1").size();
  test_support::require_equal(
      pkgsource::yaml::parse_profiles_yaml(
          document, pkgsource::source_origin("profiles.yml"), limits)
          .size(),
      std::size_t{1}, "exact scalar-byte boundary must be accepted");

  limits = {};
  limits.maximum_nodes = 12;
  test_support::require_equal(
      pkgsource::yaml::parse_profiles_yaml(
          document, pkgsource::source_origin("profiles.yml"), limits)
          .size(),
      std::size_t{1}, "exact node boundary must be accepted");

  limits = {};
  limits.maximum_depth = 6;
  test_support::require_equal(
      pkgsource::yaml::parse_profiles_yaml(
          document, pkgsource::source_origin("profiles.yml"), limits)
          .size(),
      std::size_t{1}, "exact depth boundary must be accepted");
}

} // namespace

int main()
{
  return test_support::run({
      {"rejects each resource beyond its limit",
       rejects_each_resource_beyond_its_limit},
      {"accepts each exact boundary", accepts_each_exact_boundary},
  });
}
