#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build_root=$1
project_version=$2
metadata=$build_root/meson-private/libpkgsource-yaml.pc
[ -s "$metadata" ] || metadata=$(
  find "$build_root" -type f -name libpkgsource-yaml.pc -print |
    sed -n '1p'
)
[ -n "${metadata:-}" ] && [ -s "$metadata" ] || {
  echo 'yaml-metadata-test: generated metadata not found' >&2
  exit 1
}

grep -F 'Name: libpkgsource-yaml' "$metadata" >/dev/null
grep -F "Version: $project_version" "$metadata" >/dev/null

source_count=$(awk '
  $1 == "Requires:" {
    for (i = 2; i <= NF; ++i)
      if ($i == "libpkgsource") ++count
  }
  END { print count + 0 }
' "$metadata")
[ "$source_count" -eq 1 ] || {
  echo "yaml-metadata-test: expected one public libpkgsource requirement, found $source_count" >&2
  cat "$metadata" >&2
  exit 1
}
grep -E 'Requires:.*libpkgsource[[:space:]]*>=[[:space:]]*4\.0\.0' \
  "$metadata" >/dev/null

yaml_count=$(awk '
  $1 == "Requires.private:" {
    for (i = 2; i <= NF; ++i)
      if ($i == "yaml-0.1") ++count
  }
  END { print count + 0 }
' "$metadata")
[ "$yaml_count" -eq 1 ] || {
  echo "yaml-metadata-test: expected one private yaml-0.1 requirement, found $yaml_count" >&2
  cat "$metadata" >&2
  exit 1
}
grep -E \
  'Requires.private:.*yaml-0\.1[[:space:]]*>=[[:space:]]*0\.2\.5' \
  "$metadata" >/dev/null
grep -E 'Libs:.*-lpkgsource-yaml' "$metadata" >/dev/null
