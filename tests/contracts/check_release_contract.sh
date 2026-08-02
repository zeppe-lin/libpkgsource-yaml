#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail()
{
  echo "release-contract-test: $*" >&2
  exit 1
}

grep -F "version: '1.0.0'" "$root/meson.build" >/dev/null ||
  fail 'project version is not 1.0.0'
grep -F '## 1.0.0' "$root/HISTORY.md" >/dev/null ||
  fail 'release history is not finalized'
grep -F "soversion: '1'" "$root/src/meson.build" >/dev/null ||
  fail 'library SONAME generation is not 1'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgsource dependency floor is not 3.0.0'
grep -F 'requires: [libpkgsource_dep]' "$root/src/meson.build" >/dev/null ||
  fail 'pkg-config does not promote libpkgsource by dependency object'
grep -F "choices: ['libyaml']" "$root/meson.options" >/dev/null ||
  fail 'qualified YAML provider is not explicit'
grep -F 'abi/libpkgsource-yaml.exports' "$root/src/meson.build" >/dev/null ||
  fail 'shared library does not consume the reviewed ABI manifest'
[ -s "$root/abi/libpkgsource-yaml.exports" ] || fail 'ABI manifest is missing'

if grep -F 'requires_private:' "$root/src/meson.build" >/dev/null; then
  fail 'pkg-config duplicates an implicitly private dependency'
fi
if grep -E "requires:.*['\"]libpkgsource" "$root/src/meson.build" >/dev/null; then
  fail 'pkg-config names libpkgsource as a string'
fi
if grep -R -E 'zeppe-lin\.recipe/2|RECIPE-YAML-2|recipe\.yml/2' \
    "$root/src" "$root/include" "$root/docs" "$root/README.md" >/dev/null; then
  fail 'unpublished recipe/2 generation remains'
fi
