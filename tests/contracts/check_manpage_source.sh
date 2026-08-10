#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
fail()
{
  echo "manpage-source-test: $*" >&2
  exit 1
}

check_page()
{
  page=$1
  title=$2
  name_line=$3
  source=$root/docs/man/$page.md
  [ -s "$source" ] || fail "missing canonical source: $page.md"
  [ "$(sed -n '1p' "$source")" = "$title" ] ||
    fail "title block is wrong: $page.md"
  actual_name=$(awk '
    /^# NAME$/ { in_name = 1; next }
    /^# / && in_name { exit }
    in_name && NF { print; exit }
  ' "$source")
  [ "$actual_name" = "$name_line" ] || fail "NAME section is wrong: $page.md"

  if grep -n -E '^(===+|---+)[[:space:]]*$' "$source" >/dev/null; then
    fail "Setext heading or horizontal rule in $page.md"
  fi
  if grep -n -E "^[.'](TH|SH|SS|TP|IP|PP|RS|RE|EX|EE)([[:space:]]|$)" \
      "$source" >/dev/null; then
    fail "raw roff in $page.md"
  fi
  if grep -n -E '^[[:space:]]*</?[A-Za-z][^>]*>' "$source" >/dev/null; then
    fail "raw HTML in $page.md"
  fi
  if grep -n "$(printf '\t')" "$source" >/dev/null; then
    fail "tab in $page.md"
  fi
  if grep -n -E '[[:blank:]]+$' "$source" >/dev/null; then
    fail "trailing whitespace in $page.md"
  fi
}

check_page pkgsource_yaml.3 \
  '% PKGSOURCE_YAML(3) libpkgsource-yaml 1.0.1 | libpkgsource-yaml' \
  'pkgsource_yaml - parse strict native YAML into libpkgsource declarations'
check_page pkgsource_profiles_yaml.5 \
  '% PKGSOURCE_PROFILES_YAML(5) libpkgsource-yaml 1.0.1 | libpkgsource-yaml' \
  'pkgsource_profiles_yaml - describe the native profiles YAML version 1 protocol'
check_page pkgsource_recipe_yaml.5 \
  '% PKGSOURCE_RECIPE_YAML(5) libpkgsource-yaml 1.0.1 | libpkgsource-yaml' \
  'pkgsource_recipe_yaml - describe the native recipe YAML version 1 protocol'

grep -F '```cpp' "$root/docs/man/pkgsource_yaml.3.md" >/dev/null ||
  fail 'API manual SYNOPSIS is not a C++ fenced block'
grep -F '#include <libpkgsource-yaml/libpkgsource-yaml.h>' \
  "$root/docs/man/pkgsource_yaml.3.md" >/dev/null ||
  fail 'API manual omits the umbrella header'
