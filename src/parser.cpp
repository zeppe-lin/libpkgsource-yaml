// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include <utility>

namespace pkgsource::yaml {

yaml_error::yaml_error(yaml_error_code code, std::string document,
                       std::string path, std::uint32_t line,
                       std::uint32_t column, std::string message)
    : std::runtime_error(std::move(message)), code_(code),
      document_(std::move(document)), path_(std::move(path)), line_(line),
      column_(column)
{
}

yaml_error::~yaml_error() = default;

yaml_error_code yaml_error::code() const noexcept
{
  return code_;
}

const std::string& yaml_error::document() const noexcept
{
  return document_;
}

const std::string& yaml_error::path() const noexcept
{
  return path_;
}

std::uint32_t yaml_error::line() const noexcept
{
  return line_;
}

std::uint32_t yaml_error::column() const noexcept
{
  return column_;
}

} // namespace pkgsource::yaml
