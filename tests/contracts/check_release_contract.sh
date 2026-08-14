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

grep -F "version: '2.0.0'" "$root/meson.build" >/dev/null ||
  fail 'project version is not 2.0.0'
grep -F "meson_version: '>=1.2.0'" "$root/meson.build" >/dev/null ||
  fail 'declared Meson floor is not 1.2.0'
if grep -F 'elf_export_script.full_path()' "$root/src/meson.build" >/dev/null; then
  fail 'export script path requires Meson 1.4 file.full_path()'
fi
grep -F '## 2.0.0' "$root/HISTORY.md" >/dev/null ||
  fail 'release history is not finalized'
grep -F "soversion: '2'" "$root/src/meson.build" >/dev/null ||
  fail 'library SONAME generation is not 2'
grep -F "version: ['>=4.0.0', '<5.0.0']" "$root/meson.build" >/dev/null ||
  fail 'libpkgsource dependency interval is not >=4.0.0,<5.0.0'
grep -F "version: '>=0.2.5'" "$root/meson.build" >/dev/null ||
  fail 'libyaml dependency floor is not 0.2.5'
grep -F 'requires: [libpkgsource_dep]' "$root/src/meson.build" >/dev/null ||
  fail 'pkg-config does not promote libpkgsource by dependency object'
if grep -F "'yaml_provider'" "$root/meson.options" >/dev/null; then
  fail 'release exposes a provider option without a second qualified provider'
fi
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
