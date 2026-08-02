// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/documents.h"
#include "../support/test_support.h"

#include <libpkgsource/profile.h>

#include <cstddef>
#include <string>
#include <utility>

namespace {

using test_support::require_equal;

void parses_declarations_with_exact_provenance()
{
  auto declarations = pkgsource::yaml::parse_profiles_yaml(
      test_documents::profiles, pkgsource::source_origin("profiles.yml"));

  require_equal(
      declarations.size(), std::size_t{2}, "expected two profile declarations");
  require_equal(declarations[0].name().name(),
                std::string("@toolchain"),
                "document order must be preserved");
  require_equal(declarations[0].provenance().document(),
                std::string("profiles.yml"),
                "profile provenance must retain the document label");
  require_equal(declarations[0].provenance().path(),
                std::string("profiles.toolchain"),
                "profile provenance must retain its schema path");
  require_equal(declarations[0].members()[0].provenance().path(),
                std::string("profiles.toolchain.members[0]"),
                "member provenance must retain its sequence path");
}

void equivalent_documents_seal_to_equivalent_profiles()
{
  pkgsource::profile_catalog first =
      pkgsource::profile_catalog::seal(pkgsource::yaml::parse_profiles_yaml(
          test_documents::profiles, pkgsource::source_origin("profiles.yml")));
  pkgsource::profile_catalog reordered =
      pkgsource::profile_catalog::seal(pkgsource::yaml::parse_profiles_yaml(
          test_documents::reordered_profiles,
          pkgsource::source_origin("other-profiles.yml")));

  const pkgsource::sealed_profile& toolchain =
      first.require(pkgsource::profile_reference("@toolchain"));
  require_equal(toolchain.expansion().size(),
                std::size_t{2},
                "toolchain expansion must contain two packages");
  require_equal(toolchain.expansion()[0].package().name(),
                std::string("binutils"),
                "normalized expansion must preserve package ordering");
  require_equal(toolchain.expansion()[1].package().name(),
                std::string("gcc"),
                "nested profile expansion must be retained");
  require_equal(
      toolchain.identity(),
      reordered.require(pkgsource::profile_reference("@toolchain")).identity(),
      "mapping order must not change sealed profile identity");
}

} // namespace

int main()
{
  return test_support::run({
      {"parses declarations with exact provenance",
       parses_declarations_with_exact_provenance},
      {"equivalent documents seal to equivalent profiles",
       equivalent_documents_seal_to_equivalent_profiles},
  });
}
