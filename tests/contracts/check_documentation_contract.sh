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
  DESIGN.md docs/abi.md docs/code-style.md TESTING.md \
  docs/manpage-markdown.md docs/html.md \
  docs/history/in-tree-parser-migration.md \
  docs/protocols/profiles-yaml-v1.md docs/protocols/recipe-yaml-v1.md
do
  [ -s "$root/$file" ] || fail "missing or empty: $file"
done

require "$root/README.md" '## Provider boundary'
require "$root/README.md" '## Documentation'
require "$root/DESIGN.md" '## Parser pipeline'
require "$root/DESIGN.md" '## Provider boundary'
require "$root/DESIGN.md" '## Installed documentation'
require "$root/DESIGN.md" '## HTML publication boundary'
require "$root/docs/abi.md" '## Canonical manifest'
require "$root/TESTING.md" '## Provider behavior'
require "$root/TESTING.md" '## Recipe grammar behavior'
require "$root/docs/protocols/profiles-yaml-v1.md" 'zeppe-lin.profiles/1'
require "$root/docs/protocols/recipe-yaml-v1.md" 'zeppe-lin.recipe/1'
require "$root/docs/history/in-tree-parser-migration.md" '## No compatibility layer'
require "$root/CONTRIBUTING.md" '## Boundary first'
require "$root/MAINTAINING.md" '## Release checklist'

if grep -R -E 'recipe\.yml/2|zeppe-lin\.recipe/2|RECIPE-YAML-2' \
    "$root/README.md" "$root/docs" >/dev/null; then
  fail 'discarded recipe/2 generation appears in current documentation'
fi

for duplicate in docs/architecture.md docs/testing.md
do
  [ ! -e "$root/$duplicate" ] || fail "duplicate documentation authority remains: $duplicate"
done

for retired in PROFILES-YAML.md RECIPE-YAML.md MIGRATION.md man
do
  [ ! -e "$root/$retired" ] || fail "retired root path remains: $retired"
done
