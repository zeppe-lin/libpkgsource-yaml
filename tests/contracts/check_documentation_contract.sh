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

for file in README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md \
  docs/architecture.md docs/testing.md docs/history/in-tree-parser-migration.md \
  docs/protocols/profiles-yaml-v1.md docs/protocols/recipe-yaml-v1.md CONTRIBUTING.md MAINTAINING.md
do
  [ -f "$root/$file" ] || fail "missing $file"
done
require "$root/docs/architecture.md" 'parser resource ceilings'
require "$root/docs/protocols/profiles-yaml-v1.md" 'zeppe-lin.profiles/1'
require "$root/docs/protocols/recipe-yaml-v1.md" 'zeppe-lin.recipe/1'
require "$root/docs/protocols/recipe-yaml-v1.md" 'optional `check`'
require "$root/docs/testing.md" 'document-byte, scalar-byte, node-count, and depth'
require "$root/CONTRIBUTING.md" 'must not seal profiles or sources'
require "$root/MAINTAINING.md" 'signed compatible `libpkgsource` tag'

if grep -R -E 'recipe\.yml/2|zeppe-lin\.recipe/2|RECIPE-YAML-2' \
    "$root/README.md" "$root/docs/architecture.md" "$root/docs/protocols/recipe-yaml-v1.md" \
    "$root/docs/man" >/dev/null; then
  fail 'discarded recipe/2 generation appears in current public documentation'
fi
