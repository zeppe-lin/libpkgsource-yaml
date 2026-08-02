#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
provider=$root/src/internal/yaml_event_reader_libyaml.cpp

fail()
{
  echo "provider-boundary-test: $*" >&2
  exit 1
}

[ -s "$provider" ] || fail 'libyaml provider implementation is missing'
grep -F '#include <yaml.h>' "$provider" >/dev/null ||
  fail 'selected provider does not include libyaml explicitly'

matches=$(grep -R -l -E '#include <yaml\.h>|\byaml_(parser|event|char|mark|tag|version)' \
  "$root/src" "$root/include" || true)
[ "$matches" = "$provider" ] || {
  printf '%s\n' "$matches" >&2
  fail 'libyaml types or functions escaped the provider translation unit'
}

if grep -R -E 'yaml_parser_t|yaml_event_t|yaml_event_type_t' \
    "$root/include" "$root/src/internal/document."* \
    "$root/src/profiles.cpp" "$root/src/recipe.cpp" >/dev/null; then
  fail 'provider representation escaped into grammar or public code'
fi

grep -F "choices: ['libyaml']" "$root/meson.options" >/dev/null ||
  fail 'qualified YAML provider selection is not explicit'
