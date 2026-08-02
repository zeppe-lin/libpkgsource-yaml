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
  fail 'libyaml provider does not include libyaml explicitly'

# Match provider-owned libyaml declarations and calls, not project-owned names
# such as yaml_event_reader or yaml_event_kind.
matches=$(grep -R -l -E \
  '#include <yaml\.h>|yaml_(parser_t|event_t|event_type_t|char_t)|yaml_(parser_initialize|parser_delete|parser_set_input_string|parser_parse|event_delete)|(^|[^A-Za-z0-9_])YAML_[A-Z0-9_]+' \
  "$root/src" "$root/include" || true)
[ "$matches" = "$provider" ] || {
  printf '%s\n' "$matches" >&2
  fail 'libyaml representation escaped the provider translation unit'
}

if grep -F "'yaml_provider'" "$root/meson.options" >/dev/null; then
  fail 'one-choice YAML provider option exposes configuration theatre'
fi
