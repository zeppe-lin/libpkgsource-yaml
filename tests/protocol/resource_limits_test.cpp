// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/test_support.h"

#include <cstddef>
#include <string>

namespace {

constexpr std::string_view document = "format: zeppe-lin.profiles/1\n"
                                      "profiles:\n"
                                      "  compiler:\n"
                                      "    members:\n"
                                      "      - package: gcc\n";

void expect_limit(const pkgsource::yaml::parse_limits& limits)
{
  test_support::expect_yaml_error(
      pkgsource::yaml::yaml_error_code::resource_limit,
      "profiles.yml",
      "",
      [&] {
        (void)pkgsource::yaml::parse_profiles_yaml(
            document, pkgsource::source_origin("profiles.yml"), limits);
      });
}

void expect_success(const pkgsource::yaml::parse_limits& limits,
                    std::string_view message)
{
  test_support::require_equal(
      pkgsource::yaml::parse_profiles_yaml(
          document, pkgsource::source_origin("profiles.yml"), limits)
          .size(),
      std::size_t{1},
      message);
}

void rejects_document_beyond_byte_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_document_bytes = document.size() - 1;
  expect_limit(limits);
}

void accepts_document_at_byte_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_document_bytes = document.size();
  expect_success(limits, "exact document-byte boundary must be accepted");
}

void rejects_scalar_beyond_byte_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_scalar_bytes = 8;
  expect_limit(limits);
}

void accepts_scalar_at_byte_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_scalar_bytes = std::string("zeppe-lin.profiles/1").size();
  expect_success(limits, "exact scalar-byte boundary must be accepted");
}

void rejects_node_beyond_count_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_nodes = 4;
  expect_limit(limits);
}

void accepts_node_at_count_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_nodes = 12;
  expect_success(limits, "exact node boundary must be accepted");
}

void rejects_node_beyond_depth_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_depth = 3;
  expect_limit(limits);
}

void accepts_node_at_depth_limit()
{
  pkgsource::yaml::parse_limits limits;
  limits.maximum_depth = 6;
  expect_success(limits, "exact depth boundary must be accepted");
}

} // namespace

int main()
{
  return test_support::run({
      {"rejects document beyond byte limit",
       rejects_document_beyond_byte_limit},
      {"accepts document at byte limit", accepts_document_at_byte_limit},
      {"rejects scalar beyond byte limit", rejects_scalar_beyond_byte_limit},
      {"accepts scalar at byte limit", accepts_scalar_at_byte_limit},
      {"rejects node beyond count limit", rejects_node_beyond_count_limit},
      {"accepts node at count limit", accepts_node_at_count_limit},
      {"rejects node beyond depth limit", rejects_node_beyond_depth_limit},
      {"accepts node at depth limit", accepts_node_at_depth_limit},
  });
}
