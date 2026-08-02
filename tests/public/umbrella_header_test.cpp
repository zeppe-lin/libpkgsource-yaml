// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/libpkgsource-yaml.h>

int main()
{
  pkgsource::yaml::parse_limits limits;
  return limits.maximum_depth == 0 ? 1 : 0;
}
