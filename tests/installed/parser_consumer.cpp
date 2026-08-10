// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

#include <cstddef>

int main()
{
  const auto declarations = pkgsource::yaml::parse_profiles_yaml(
      "format: zeppe-lin.profiles/1\nprofiles: {}\n",
      pkgsource::source_origin("installed-consumer.yml"));
  return declarations.size() == std::size_t{0} ? 0 : 1;
}
