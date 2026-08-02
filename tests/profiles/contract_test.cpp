// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include <cassert>
#include <functional>
#include <string>
#include <string_view>

using namespace pkgsource;
using namespace pkgsource::yaml;

namespace {

template <typename Function>
void expect_yaml(yaml_error_code code, Function&& function,
                 std::string_view path = {})
{
  try {
    function();
    assert(false);
  } catch (const yaml_error& value) {
    assert(value.code() == code);
    assert(value.document() == "profiles.yml");
    assert(value.line() > 0);
    assert(value.column() > 0);
    if (!path.empty())
      assert(value.path() == path);
  }
}

const char* first_document()
{
  return R"(format: zeppe-lin.profiles/1
profiles:
  toolchain:
    members:
      - profile: "@compiler"
      - package: binutils
  compiler:
    members:
      - package: gcc
)";
}

const char* reordered_document()
{
  return R"(profiles:
  compiler:
    members:
      - package: gcc
  toolchain:
    members:
      - package: binutils
      - profile: "@compiler"
format: zeppe-lin.profiles/1
)";
}

void test_parse_and_seal()
{
  auto declarations = parse_profiles_yaml(
      first_document(), source_origin("profiles.yml"));
  assert(declarations.size() == 2);
  assert(declarations[0].name().name() == "@toolchain");
  assert(declarations[0].provenance().document() == "profiles.yml");
  assert(declarations[0].provenance().path() == "profiles.toolchain");
  assert(declarations[0].members()[0].provenance().path()
         == "profiles.toolchain.members[0]");

  profile_catalog first = profile_catalog::seal(std::move(declarations));
  profile_catalog reordered = profile_catalog::seal(parse_profiles_yaml(
      reordered_document(), source_origin("profiles.yml")));
  const sealed_profile& toolchain = first.require(
      profile_reference("@toolchain"));
  assert(toolchain.expansion().size() == 2);
  assert(toolchain.expansion()[0].package().name() == "binutils");
  assert(toolchain.expansion()[1].package().name() == "gcc");
  assert(toolchain.identity()
         == reordered.require(profile_reference("@toolchain")).identity());
}

void test_strict_yaml_rejections()
{
  expect_yaml(yaml_error_code::duplicate_key, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "format: zeppe-lin.profiles/1\n"
        "profiles: {}\n",
        source_origin("profiles.yml"));
  }, "format");

  expect_yaml(yaml_error_code::unknown_key, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\nprofiles: {}\nextra: no\n",
        source_origin("profiles.yml"));
  }, "extra");

  expect_yaml(yaml_error_code::unsupported_feature, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  compiler: &compiler\n"
        "    members:\n"
        "      - package: gcc\n",
        source_origin("profiles.yml"));
  });

  expect_yaml(yaml_error_code::unsupported_feature, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\nprofiles: *compiler\n",
        source_origin("profiles.yml"));
  });

  expect_yaml(yaml_error_code::unsupported_feature, [] {
    (void)parse_profiles_yaml(
        "%YAML 1.2\n---\n"
        "format: zeppe-lin.profiles/1\nprofiles: {}\n",
        source_origin("profiles.yml"));
  });

  expect_yaml(yaml_error_code::unsupported_feature, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  compiler:\n"
        "    <<: {members: [{package: gcc}]}\n",
        source_origin("profiles.yml"));
  });

  expect_yaml(yaml_error_code::unsupported_feature, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "profiles: !zeppe/profiles {}\n",
        source_origin("profiles.yml"));
  });

  expect_yaml(yaml_error_code::invalid_document, [] {
    (void)parse_profiles_yaml(
        "---\nformat: zeppe-lin.profiles/1\nprofiles: {}\n"
        "---\nformat: zeppe-lin.profiles/1\nprofiles: {}\n",
        source_origin("profiles.yml"));
  });

  expect_yaml(yaml_error_code::invalid_type, [] {
    (void)parse_profiles_yaml(
        "? [format]\n: zeppe-lin.profiles/1\nprofiles: {}\n",
        source_origin("profiles.yml"));
  }, "$");

  expect_yaml(yaml_error_code::invalid_document, [] {
    (void)parse_profiles_yaml("", source_origin("profiles.yml"));
  }, "$");

  expect_yaml(yaml_error_code::syntax, [] {
    const std::string invalid_utf8("\xff", 1);
    (void)parse_profiles_yaml(invalid_utf8, source_origin("profiles.yml"));
  }, "$");
}

void test_schema_rejections()
{
  expect_yaml(yaml_error_code::invalid_value, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/2\nprofiles: {}\n",
        source_origin("profiles.yml"));
  }, "format");

  expect_yaml(yaml_error_code::invalid_type, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\nprofiles: []\n",
        source_origin("profiles.yml"));
  }, "profiles");

  expect_yaml(yaml_error_code::invalid_value, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  Toolchain:\n"
        "    members:\n"
        "      - package: gcc\n",
        source_origin("profiles.yml"));
  }, "profiles.Toolchain");

  expect_yaml(yaml_error_code::invalid_value, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  empty:\n"
        "    members: []\n",
        source_origin("profiles.yml"));
  }, "profiles.empty");

  expect_yaml(yaml_error_code::invalid_type, [] {
    (void)parse_profiles_yaml(
        "format: zeppe-lin.profiles/1\n"
        "profiles:\n"
        "  compiler:\n"
        "    members:\n"
        "      - gcc\n",
        source_origin("profiles.yml"));
  }, "profiles.compiler.members[0]");
}

} // namespace

int main()
{
  test_parse_and_seal();
  test_strict_yaml_rejections();
  test_schema_rejections();
}
