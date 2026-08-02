#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

prefix=$1
version=$2
root=$prefix/share/htmldocs/libpkgsource-yaml/$version

fail()
{
  echo "installed-html-docs-test: $*" >&2
  exit 1
}

for file in \
  index.html \
  architecture.html \
  abi.html \
  manual/pkgsource_yaml.3.html \
  manual/pkgsource_profiles_yaml.5.html \
  manual/pkgsource_recipe_yaml.5.html \
  protocols/profiles-yaml-v1.html \
  protocols/recipe-yaml-v1.html \
  api/index.html \
  assets/house.css \
  legal/COPYING \
  legal/COPYRIGHT
do
  [ -s "$root/$file" ] || fail "missing installed HTML artifact: $file"
done

find "$root" -type f -name '*.md' | grep . >/dev/null &&
  fail 'canonical Markdown escaped into the rendered HTML tree'
exit 0
