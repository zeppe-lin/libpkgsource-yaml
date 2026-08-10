#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1

fail()
{
  echo "test-layout-contract-test: $*" >&2
  exit 1
}

for directory in mechanism protocol integration header installed support contracts; do
  [ -d "$root/tests/$directory" ] || fail "missing tests/$directory"
done
for obsolete in internal parser profiles recipe public; do
  [ ! -d "$root/tests/$obsolete" ] || fail "obsolete tests/$obsolete taxonomy remains"
done
[ -s "$root/tests/installed/parser_consumer.cpp" ] ||
  fail 'installed parser consumer is missing'
grep -F 'parse_profiles_yaml(' "$root/tests/installed/parser_consumer.cpp" >/dev/null ||
  fail 'installed consumer does not exercise the parser library'
