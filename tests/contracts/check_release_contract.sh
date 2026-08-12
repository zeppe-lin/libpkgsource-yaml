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

grep -F "version: '1.0.1'" "$root/meson.build" >/dev/null ||
  fail 'project version is not 1.0.1'
grep -F "meson_version: '>=1.2.0'" "$root/meson.build" >/dev/null ||
  fail 'declared Meson floor is not 1.2.0'
if grep -F 'elf_export_script.full_path()' "$root/src/meson.build" >/dev/null; then
  fail 'export script path requires Meson 1.4 file.full_path()'
fi
grep -F '## 1.0.1' "$root/HISTORY.md" >/dev/null ||
  fail 'release history is not finalized'
grep -F "soversion: '1'" "$root/src/meson.build" >/dev/null ||
  fail 'library SONAME generation is not 1'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null ||
  fail 'libpkgsource dependency floor is not 3.0.0'
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

provider_commit=2c891fc7a770e8ba2fec34fc6b545c672beb37e6
[ "$(grep -F -c 'ref: v3.0.1' "$root/.github/workflows/ci.yml")" -eq 2 ] ||
  fail 'hosted matrices do not both use current libpkgsource 3.0.1'
[ "$(grep -F -c 'repository: yaml/libyaml' "$root/.github/workflows/ci.yml")" -eq 2 ] ||
  fail 'hosted matrices do not both use upstream libyaml'
[ "$(grep -F -c "ref: $provider_commit" "$root/.github/workflows/ci.yml")" -eq 2 ] ||
  fail 'hosted matrices do not both pin the qualified libyaml 0.2.5 commit'
if grep -F 'libyaml-dev' "$root/.github/workflows/ci.yml" >/dev/null; then
  fail 'hosted qualification inherits libyaml headers from the runner distribution'
fi
