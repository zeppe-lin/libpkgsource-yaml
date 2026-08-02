// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include <cassert>
#include <functional>
#include <string>

using namespace pkgsource;
using namespace pkgsource::yaml;

namespace {

template <typename Function>
void expect_limit(Function&& function)
{
  try {
    function();
    assert(false);
  } catch (const yaml_error& failure) {
    assert(failure.code() == yaml_error_code::resource_limit);
    assert(failure.line() > 0);
    assert(failure.column() > 0);
  }
}

const std::string document =
    "format: zeppe-lin.profiles/1\n"
    "profiles:\n"
    "  compiler:\n"
    "    members:\n"
    "      - package: gcc\n";

void test_document_limit()
{
  parse_limits limits;
  limits.maximum_document_bytes = document.size() - 1;
  expect_limit([&] {
    (void)parse_profiles_yaml(
        document, source_origin("profiles.yml"), limits);
  });
}

void test_scalar_limit()
{
  parse_limits limits;
  limits.maximum_scalar_bytes = 8;
  expect_limit([&] {
    (void)parse_profiles_yaml(
        document, source_origin("profiles.yml"), limits);
  });
}

void test_node_limit()
{
  parse_limits limits;
  limits.maximum_nodes = 4;
  expect_limit([&] {
    (void)parse_profiles_yaml(
        document, source_origin("profiles.yml"), limits);
  });
}

void test_depth_limit()
{
  parse_limits limits;
  limits.maximum_depth = 3;
  expect_limit([&] {
    (void)parse_profiles_yaml(
        document, source_origin("profiles.yml"), limits);
  });
}

void test_exact_document_boundary()
{
  parse_limits limits;
  limits.maximum_document_bytes = document.size();
  const auto declarations = parse_profiles_yaml(
      document, source_origin("profiles.yml"), limits);
  assert(declarations.size() == 1);
}


void test_exact_scalar_boundary()
{
  parse_limits limits;
  limits.maximum_scalar_bytes = std::string("zeppe-lin.profiles/1").size();
  const auto declarations = parse_profiles_yaml(
      document, source_origin("profiles.yml"), limits);
  assert(declarations.size() == 1);
}

void test_exact_node_boundary()
{
  parse_limits limits;
  limits.maximum_nodes = 12;
  const auto declarations = parse_profiles_yaml(
      document, source_origin("profiles.yml"), limits);
  assert(declarations.size() == 1);
}

void test_exact_depth_boundary()
{
  parse_limits limits;
  limits.maximum_depth = 6;
  const auto declarations = parse_profiles_yaml(
      document, source_origin("profiles.yml"), limits);
  assert(declarations.size() == 1);
}

} // namespace

int main()
{
  test_document_limit();
  test_scalar_limit();
  test_node_limit();
  test_depth_limit();
  test_exact_document_boundary();
  test_exact_scalar_boundary();
  test_exact_node_boundary();
  test_exact_depth_boundary();
}
