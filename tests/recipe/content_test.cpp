// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include "../support/documents.h"
#include "../support/test_support.h"

#include <libpkgsource/profile.h>
#include <libpkgsource/snapshot.h>

#include <cstddef>
#include <string>
#include <utility>

namespace {

using test_support::require;
using test_support::require_equal;

pkgsource::profile_catalog profiles()
{
  return pkgsource::profile_catalog::seal(pkgsource::yaml::parse_profiles_yaml(
      test_documents::profiles, pkgsource::source_origin("profiles.yml")));
}

void parses_complete_recipe_declaration()
{
  pkgsource::recipe_declaration declaration =
      pkgsource::yaml::parse_recipe_yaml(
          test_documents::complete_recipe,
          pkgsource::source_origin("recipe.yml"));

  require_equal(declaration.release().package().name(),
                std::string("example"),
                "package name must be parsed");
  require_equal(declaration.provenance().document(),
                std::string("recipe.yml"),
                "recipe provenance must retain the document label");
  require_equal(declaration.provenance().path(),
                std::string("document"),
                "recipe provenance must identify the document root");
  require_equal(declaration.requirements()[0].provenance().path(),
                std::string("requirements.build[0]"),
                "requirement provenance must retain its sequence path");
  require(declaration.check_program().has_value(),
          "optional check program must be present");
  require_equal(declaration.check_program()->material(),
                std::string("meson test -C build\n"),
                "program material must remain byte-exact");
}

void parsed_declaration_seals_complete_source_authority()
{
  const pkgsource::profile_catalog catalog = profiles();
  pkgsource::source_snapshot snapshot =
      pkgsource::seal_source(pkgsource::source_origin("recipe.yml"),
                             pkgsource::yaml::parse_recipe_yaml(
                                 test_documents::complete_recipe,
                                 pkgsource::source_origin("recipe.yml")),
                             catalog);

  const pkgsource::sealed_recipe& recipe = snapshot.recipe();
  require_equal(recipe.release().version_release(),
                std::string("1.2.3-1"),
                "release value must cross the parser boundary");
  require(recipe.metadata().description().has_value(),
          "optional description must be retained");
  require_equal(recipe.sources().size(),
                std::size_t{2},
                "both source inputs must be retained");
  require_equal(recipe.build_requirements().size(),
                std::size_t{3},
                "profile expansion and package build requirements must seal");
  require_equal(recipe.run_requirements().size(),
                std::size_t{1},
                "runtime requirements must seal");
  require_equal(recipe.check_requirements().size(),
                std::size_t{1},
                "check requirements must seal");
  require(recipe.check_program().has_value(), "check program must seal");
  require_equal(
      recipe.lifecycle_requirements(pkgsource::lifecycle_action::post_install)
          .size(),
      std::size_t{1},
      "lifecycle requirements must seal");
  require(recipe.lifecycle(pkgsource::lifecycle_action::post_install) !=
              nullptr,
          "lifecycle program must seal");
  require_equal(recipe.architectures().target()[0].name(),
                std::string("x86_64"),
                "target architecture must seal");
}

void equivalent_yaml_order_preserves_source_identity()
{
  const pkgsource::profile_catalog catalog = profiles();
  pkgsource::source_snapshot first =
      pkgsource::seal_source(pkgsource::source_origin("recipe.yml"),
                             pkgsource::yaml::parse_recipe_yaml(
                                 test_documents::complete_recipe,
                                 pkgsource::source_origin("recipe.yml")),
                             catalog);
  pkgsource::source_snapshot reordered = pkgsource::seal_source(
      pkgsource::source_origin("other.yml"),
      pkgsource::yaml::parse_recipe_yaml(test_documents::reordered_recipe,
                                         pkgsource::source_origin("other.yml")),
      catalog);

  require_equal(first.identity(),
                reordered.identity(),
                "mapping order and document label must not change authority");
}

void accepts_check_program_without_check_requirements()
{
  const pkgsource::profile_catalog catalog = profiles();
  pkgsource::source_snapshot snapshot =
      pkgsource::seal_source(pkgsource::source_origin("recipe.yml"),
                             pkgsource::yaml::parse_recipe_yaml(
                                 R"(format: zeppe-lin.recipe/1
package:
  name: checked
  version: 2.0
  release: 1
  summary: Checked package
  licenses: [MIT]
requirements: {}
sources: []
build: {language: posix-shell, script: "true\n"}
check: {language: posix-shell, script: "true\n"}
)",
                                 pkgsource::source_origin("recipe.yml")),
                             catalog);

  require(snapshot.recipe().check_program().has_value(),
          "check program must be present");
  require(snapshot.recipe().check_requirements().empty(),
          "check requirements may remain empty");
}

} // namespace

int main()
{
  return test_support::run({
      {"parses complete recipe declaration",
       parses_complete_recipe_declaration},
      {"parsed declaration seals complete source authority",
       parsed_declaration_seals_complete_source_authority},
      {"equivalent YAML order preserves source identity",
       equivalent_yaml_order_preserves_source_identity},
      {"accepts check program without check requirements",
       accepts_check_program_without_check_requirements},
  });
}
