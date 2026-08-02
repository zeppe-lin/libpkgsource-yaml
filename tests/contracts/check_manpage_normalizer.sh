#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1
canonicalizer=$root/tools/canonicalize-man-roff.awk

fail()
{
  echo "manpage-normalizer-test: $*" >&2
  exit 1
}

[ -s "$canonicalizer" ] || fail 'roff canonicalizer is missing or empty'

input=$(mktemp)
expected=$(mktemp)
actual=$(mktemp)
trap 'rm -f "$input" "$expected" "$actual"' EXIT HUP INT TERM

cat > "$input" <<'EOF'
.SH SYNOPSIS
.EX
#include \f[B]<libpkgsource\-plan/libpkgsource\-plan.h>\f[R]
.EE
.IP \(bu 2
newer Pandoc bullet spelling
.IP \[bu] 2
canonical bullet spelling
EOF

cat > "$expected" <<'EOF'
.SH SYNOPSIS
.EX
#include <libpkgsource\-plan/libpkgsource\-plan.h>
.EE
.IP \[bu] 2
newer Pandoc bullet spelling
.IP \[bu] 2
canonical bullet spelling
EOF

awk -f "$canonicalizer" "$input" > "$actual"

if ! cmp -s "$expected" "$actual"; then
  diff -u "$expected" "$actual" || true
  fail 'equivalent Pandoc 3.x output is not canonicalized identically'
fi

awk -f "$canonicalizer" "$actual" > "$input"
if ! cmp -s "$actual" "$input"; then
  diff -u "$actual" "$input" || true
  fail 'roff canonicalization is not idempotent'
fi
