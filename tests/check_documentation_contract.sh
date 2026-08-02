#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail()
{
  echo "documentation-contract-test: $*" >&2
  exit 1
}
require()
{
  file=$1
  text=$2
  grep -F -- "$text" "$file" >/dev/null ||
    fail "${file#$root/} omits: $text"
}

for file in README.md DESIGN.md HISTORY.md MIGRATION.md TESTING.md \
  PROFILES-YAML.md RECIPE-YAML.md CONTRIBUTING.md MAINTAINING.md
do
  [ -f "$root/$file" ] || fail "missing $file"
done
require "$root/DESIGN.md" 'parser resource ceilings'
require "$root/PROFILES-YAML.md" 'zeppe-lin.profiles/1'
require "$root/RECIPE-YAML.md" 'zeppe-lin.recipe/1'
require "$root/RECIPE-YAML.md" 'optional `check`'
require "$root/TESTING.md" 'document-byte, scalar-byte, node-count, and depth'
require "$root/CONTRIBUTING.md" 'must not seal profiles or sources'
require "$root/MAINTAINING.md" 'signed compatible `libpkgsource` tag'

if grep -R -E 'recipe\.yml/2|zeppe-lin\.recipe/2|RECIPE-YAML-2' \
    "$root/README.md" "$root/DESIGN.md" "$root/RECIPE-YAML.md" \
    "$root/man" >/dev/null; then
  fail 'discarded recipe/2 generation appears in current public documentation'
fi
