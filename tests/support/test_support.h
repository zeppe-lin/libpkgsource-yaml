// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <libpkgsource-yaml/parser.h>

#include <libpkgsource/error.h>

#include <exception>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace test_support {

class failure final : public std::runtime_error {
public:
  explicit failure(std::string message) : std::runtime_error(std::move(message))
  {
  }
};

inline void require(bool condition, std::string_view message)
{
  if (!condition) {
    throw failure(std::string(message));
  }
}

template <typename Left, typename Right>
void require_equal(const Left& left,
                   const Right& right,
                   std::string_view message)
{
  require(left == right, message);
}

template <typename Function>
void expect_yaml_error(pkgsource::yaml::yaml_error_code expected_code,
                       std::string_view expected_document,
                       std::string_view expected_path,
                       Function&& function)
{
  try {
    function();
  } catch (const pkgsource::yaml::yaml_error& error) {
    require(error.code() == expected_code, "unexpected YAML error category");
    require_equal(error.document(),
                  std::string(expected_document),
                  "unexpected YAML error document");
    require(error.line() > 0, "YAML error line must be one-based");
    require(error.column() > 0, "YAML error column must be one-based");
    if (!expected_path.empty()) {
      require_equal(error.path(),
                    std::string(expected_path),
                    "unexpected YAML error path");
    }
    return;
  }
  throw failure("expected yaml_error was not thrown");
}

template <typename Function>
void expect_core_error(pkgsource::error_code expected_code, Function&& function)
{
  try {
    function();
  } catch (const pkgsource::error& error) {
    require(error.code() == expected_code,
            "unexpected libpkgsource error category");
    return;
  }
  throw failure("expected pkgsource::error was not thrown");
}

struct test_case final {
  std::string_view name;
  void (*body)();
};

inline int run(std::initializer_list<test_case> cases)
{
  for (const test_case& current : cases) {
    try {
      current.body();
    } catch (const std::exception& error) {
      std::cerr << "FAIL: " << current.name << ": " << error.what() << '\n';
      return 1;
    } catch (...) {
      std::cerr << "FAIL: " << current.name << ": non-standard exception\n";
      return 1;
    }
  }
  return 0;
}

} // namespace test_support
