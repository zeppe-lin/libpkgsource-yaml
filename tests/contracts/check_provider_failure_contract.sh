#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
provider=$root/src/internal/yaml_event_reader_libyaml.cpp

fail()
{
  echo "provider-failure-contract-test: $*" >&2
  exit 1
}

grep -F 'throw std::bad_alloc();' "$provider" >/dev/null ||
  fail 'provider allocation failure is not preserved as std::bad_alloc'
grep -F 'case YAML_MEMORY_ERROR:' "$provider" >/dev/null ||
  fail 'libyaml parse-time memory failure is not distinguished'
for category in YAML_READER_ERROR YAML_SCANNER_ERROR YAML_PARSER_ERROR; do
  grep -F "case $category:" "$provider" >/dev/null ||
    fail "provider syntax domain omits $category"
done
grep -F 'libyaml parser failed outside parser error domain' "$provider" >/dev/null ||
  fail 'impossible provider failures are not quarantined as provider defects'
