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
  docs/architecture.md docs/abi.md docs/code-style.md docs/testing.md \
  docs/manpage-markdown.md docs/html.md \
  docs/history/in-tree-parser-migration.md \
  docs/protocols/profiles-yaml-v1.md docs/protocols/recipe-yaml-v1.md
do
  [ -s "$root/$file" ] || fail "missing or empty: $file"
done

require "$root/README.md" '## Provider boundary'
require "$root/README.md" '## Documentation'
require "$root/docs/architecture.md" '## Parser pipeline'
require "$root/docs/architecture.md" '## Provider boundary'
require "$root/docs/architecture.md" '## Installed documentation'
require "$root/docs/architecture.md" '## HTML publication boundary'
require "$root/docs/abi.md" '## Canonical manifest'
require "$root/docs/testing.md" '## Provider behavior'
require "$root/docs/testing.md" '## Recipe grammar behavior'
require "$root/docs/protocols/profiles-yaml-v1.md" 'zeppe-lin.profiles/1'
require "$root/docs/protocols/recipe-yaml-v1.md" 'zeppe-lin.recipe/1'
require "$root/docs/history/in-tree-parser-migration.md" '## No compatibility layer'
require "$root/CONTRIBUTING.md" '## Boundary first'
require "$root/MAINTAINING.md" '## Release checklist'

if grep -R -E 'recipe\.yml/2|zeppe-lin\.recipe/2|RECIPE-YAML-2' \
    "$root/README.md" "$root/docs" >/dev/null; then
  fail 'discarded recipe/2 generation appears in current documentation'
fi

for retired in DESIGN.md TESTING.md PROFILES-YAML.md RECIPE-YAML.md MIGRATION.md man
do
  [ ! -e "$root/$retired" ] || fail "retired root path remains: $retired"
done
