#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
header=$root/include/libpkgsource-yaml/parser.h
config=$root/Doxyfile

fail()
{
  echo "doxygen-contract-test: $*" >&2
  exit 1
}

[ -s "$header" ] || fail 'public parser header is missing'
[ -s "$config" ] || fail 'Doxygen configuration is missing'

for setting in \
  'WARN_IF_UNDOCUMENTED   = YES' \
  'WARN_IF_DOC_ERROR      = YES' \
  'WARN_NO_PARAMDOC       = YES' \
  'WARN_AS_ERROR          = YES'
do
  grep -F "$setting" "$config" >/dev/null ||
    fail "Doxygen strictness is missing: $setting"
done

for parameter in code document path line column message
do
  grep -F "@param $parameter " "$header" >/dev/null ||
    fail "yaml_error constructor parameter is undocumented: $parameter"
done
