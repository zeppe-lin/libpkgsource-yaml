#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail()
{
  echo "parser-boundary-test: $*" >&2
  exit 1
}

if grep -R -E 'seal_source|seal_recipe|profile_catalog::seal' \
    "$root/src" "$root/include" >/dev/null; then
  fail 'parser implementation crosses the semantic sealing boundary'
fi
if grep -R -E 'parse_(profiles|recipe)_yaml_v[0-9]|seal_(profiles|recipe)_yaml' \
    "$root/src" "$root/include" >/dev/null; then
  fail 'public or private parser API retains experimental generation names'
fi
if grep -R -E 'recipe_yaml_v[0-9]|source_syntax|recipe_identity' \
    "$root/src" "$root/include" >/dev/null; then
  fail 'parser implementation imports removed core syntax authority'
fi
grep -F 'parse_profiles_yaml(' "$root/include/libpkgsource-yaml/parser.h" >/dev/null ||
  fail 'profile parser entry point is missing'
grep -F 'parse_recipe_yaml(' "$root/include/libpkgsource-yaml/parser.h" >/dev/null ||
  fail 'recipe parser entry point is missing'
grep -F 'resource_limit' "$root/include/libpkgsource-yaml/parser.h" >/dev/null ||
  fail 'bounded parser failure category is missing'

if grep -R -E 'libpkgsource-codec|libpkgsource-plan|libpkgplan'     "$root/src" "$root/include" >/dev/null; then
  fail 'parser imports unrelated adapter or planner authority'
fi
