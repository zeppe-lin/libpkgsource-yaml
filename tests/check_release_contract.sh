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
  fail 'pkg-config does not promote libpkgsource through its dependency object'
if grep -F 'requires_private:' "$root/src/meson.build" >/dev/null; then
  fail 'pkg-config duplicates an implicitly private dependency'
fi
if grep -E "requires:.*['\"]libpkgsource" \
    "$root/src/meson.build" >/dev/null; then
  fail 'pkg-config names libpkgsource as a string instead of de-duplicating its dependency object'
fi
grep -F 'zeppe-lin.recipe/1' "$root/RECIPE-YAML.md" >/dev/null ||
  fail 'recipe/1 protocol is not documented'
if grep -R -E 'zeppe-lin\.recipe/2|RECIPE-YAML-2|recipe\.yml/2' \
    "$root/src" "$root/include" "$root/man" "$root/README.md" \
    "$root/DESIGN.md" "$root/RECIPE-YAML.md" >/dev/null; then
  fail 'unpublished recipe/2 generation remains in the public implementation or contract'
fi
