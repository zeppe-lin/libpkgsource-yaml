#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

prefix=$1
project=libpkgsource-yaml
docdir=$prefix/share/doc/$project
man3=$prefix/share/man/man3
man5=$prefix/share/man/man5

fail()
{
  echo "installed-docs-test: $*" >&2
  exit 1
}

for file in \
  README.md \
  HISTORY.md \
  CONTRIBUTING.md \
  MAINTAINING.md \
  COPYING \
  COPYRIGHT \
  architecture.md \
  abi.md \
  code-style.md \
  manpage-markdown.md \
  testing.md \
  html.md \
  protocols/profiles-yaml-v1.md \
  protocols/recipe-yaml-v1.md \
  history/in-tree-parser-migration.md \
  man/pkgsource_yaml.3.md \
  man/pkgsource_profiles_yaml.5.md \
  man/pkgsource_recipe_yaml.5.md \
  assets/house.css \
  assets/doxygen-extra.css
do
  [ -s "$docdir/$file" ] || fail "missing installed documentation: $file"
done

[ -s "$man3/pkgsource_yaml.3" ] || fail 'pkgsource_yaml(3) is not installed'
[ -s "$man5/pkgsource_profiles_yaml.5" ] || fail 'profiles protocol manual is not installed'
[ -s "$man5/pkgsource_recipe_yaml.5" ] || fail 'recipe protocol manual is not installed'

if find "$docdir" -type f \( -name meson.build -o -path '*/generated/*' \) |
    grep . >/dev/null; then
  fail 'build metadata or derived roff escaped into canonical documentation'
fi
