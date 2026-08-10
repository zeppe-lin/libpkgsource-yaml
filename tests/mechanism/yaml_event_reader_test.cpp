// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include "../support/test_support.h"
#include "internal/yaml_event_reader.h"

#include <array>
#include <cstddef>
#include <string>

namespace {

using pkgsource::yaml::internal::yaml_event_kind;
using pkgsource::yaml::internal::yaml_event_reader;
using test_support::require;
using test_support::require_equal;

void normalizes_provider_events()
{
  yaml_event_reader reader("key: value\n", "document.yml", 32);
  constexpr std::array expected{
      yaml_event_kind::stream_start,
      yaml_event_kind::document_start,
      yaml_event_kind::mapping_start,
      yaml_event_kind::scalar,
      yaml_event_kind::scalar,
      yaml_event_kind::mapping_end,
      yaml_event_kind::document_end,
      yaml_event_kind::stream_end,
  };

  for (yaml_event_kind kind : expected) {
    const auto event = reader.next();
    require(event.kind == kind, "provider event kind was not normalized");
    require(event.mark.line > 0, "provider line must be one-based");
    require(event.mark.column > 0, "provider column must be one-based");
  }
}

void retains_scalar_bytes_and_tags()
{
  yaml_event_reader reader("key: value\n", "document.yml", 32);
  (void)reader.next();
  (void)reader.next();
  (void)reader.next();
  const auto key = reader.next();
  const auto value = reader.next();

  require_equal(
      key.value, std::string("key"), "mapping key bytes must be retained");
  require_equal(value.value,
                std::string("value"),
                "mapping value bytes must be retained");
  require(key.anchor.empty(), "plain scalar must have no anchor");
}

void enforces_scalar_limit_before_tree_construction()
{
  test_support::expect_yaml_error(
      pkgsource::yaml::yaml_error_code::resource_limit,
      "document.yml",
      "$",
      [] {
        yaml_event_reader reader("value: oversized\n", "document.yml", 4);
        while (reader.next().kind != yaml_event_kind::stream_end) {
        }
      });
}

} // namespace

int main()
{
  return test_support::run({
      {"normalizes provider events", normalizes_provider_events},
      {"retains scalar bytes and tags", retains_scalar_bytes_and_tags},
      {"enforces scalar limit before tree construction",
       enforces_scalar_limit_before_tree_construction},
  });
}
