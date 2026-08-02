#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
header=$root/include/libpkgsource-yaml/parser.h
config=$root/Doxyfile

fail()
{
  echo "doxygen-contract-test: $*" >&2
  exit 1
}

[ -s "$header" ] || fail 'public parser header is missing'
[ -s "$config" ] || fail 'Doxygen configuration is missing'

for setting in \
  'WARN_IF_UNDOCUMENTED   = YES' \
  'WARN_IF_DOC_ERROR      = YES' \
  'WARN_NO_PARAMDOC       = YES' \
  'WARN_AS_ERROR          = YES'
do
  grep -F "$setting" "$config" >/dev/null ||
    fail "Doxygen strictness is missing: $setting"
done

for parameter in code document path line column message
do
  grep -F "@param $parameter " "$header" >/dev/null ||
    fail "yaml_error constructor parameter is undocumented: $parameter"
done

for observer in code document path line column
do
  observer_line=$(grep -n " $observer() const noexcept;" "$header" | cut -d: -f1)
  [ -n "$observer_line" ] || fail "yaml_error observer is missing: $observer"

  first_line=$((observer_line - 8))
  [ "$first_line" -gt 0 ] || first_line=1
  sed -n "${first_line},${observer_line}p" "$header" |
    grep -F '@return ' >/dev/null ||
    fail "yaml_error observer return is undocumented: $observer"
done

for function in parse_profiles_yaml parse_recipe_yaml
do
  function_line=$(grep -n "^$function(" "$header" | cut -d: -f1)
  [ -n "$function_line" ] || fail "public parser function is missing: $function"

  first_line=$((function_line - 24))
  [ "$first_line" -gt 0 ] || first_line=1
  sed -n "${first_line},${function_line}p" "$header" |
    grep -F '@return ' >/dev/null ||
    fail "public parser return is undocumented: $function"
done
