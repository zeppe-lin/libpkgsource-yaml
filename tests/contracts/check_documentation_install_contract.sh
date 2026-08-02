#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
build=$root/docs/meson.build
man_build=$root/docs/man/meson.build

fail()
{
  echo "documentation-install-contract-test: $*" >&2
  exit 1
}

[ -s "$build" ] || fail 'docs/meson.build is missing'
[ -s "$man_build" ] || fail 'docs/man/meson.build is missing'

grep -F "get_option('datadir') / 'doc' / meson.project_name()" "$build" \
  >/dev/null || fail 'canonical documentation install root is not project-owned'
grep -F "install_tag: 'doc'" "$build" >/dev/null ||
  fail 'canonical documentation is not assigned to the doc install tag'
grep -F "'man/pkgsource_yaml.3.md'" "$build" >/dev/null ||
  fail 'canonical manual source is not installed as project documentation'
grep -F "'html.md'" "$build" >/dev/null ||
  fail 'HTML documentation policy is not installed'
grep -F "'assets/house.css'" "$build" >/dev/null ||
  fail 'HTML documentation source assets are not installed'
grep -F "install_tag: 'man'" "$man_build" >/dev/null ||
  fail 'generated manual page is not assigned to the man install tag'

if grep -F 'install_subdir(' "$build" >/dev/null; then
  fail 'documentation installation copies an unreviewed source subtree'
fi
