#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1

fail()
{
  echo "manpage-writer-options-test: $*" >&2
  exit 1
}

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM
mkdir -p "$work/root/tools" "$work/root/docs/man"
cp "$root/tools/update-man-pages.sh" "$work/root/tools/"
cp "$root/tools/canonicalize-man-roff.awk" "$work/root/tools/"
for page in pkgsource_yaml.3 pkgsource_profiles_yaml.5 pkgsource_recipe_yaml.5
do
  : > "$work/root/docs/man/$page.md"
done

cat > "$work/pandoc" <<'SHIM'
#!/bin/sh
set -eu
case ${1-} in
  --version)
    case ${PANDOC_TEST_MODE-} in
      legacy) echo 'pandoc 3.1.11.1' ;;
      current) echo 'pandoc 3.8.2' ;;
      *) exit 2 ;;
    esac
    exit 0
    ;;
  --help)
    case ${PANDOC_TEST_MODE-} in
      legacy) echo '  --no-highlight' ;;
      current) echo '  --syntax-highlighting=STYLE' ;;
      *) exit 2 ;;
    esac
    exit 0
    ;;
esac
printf '%s\n' "$@" >> "$PANDOC_TEST_LOG"
printf '%s\n' '.TH "TEST" "1"' '.SH NAME' 'test'
SHIM
chmod +x "$work/pandoc"

run_case()
{
  mode=$1
  expected=$2
  forbidden=$3
  log=$work/$mode.log
  : > "$log"
  PANDOC_TEST_MODE=$mode PANDOC_TEST_LOG=$log \
    "$work/root/tools/update-man-pages.sh" \
      --write "$work/pandoc" "$work/root"

  count=$(grep -Fxc -- "$expected" "$log" || true)
  [ "$count" -eq 3 ] ||
    fail "$mode writer option appeared $count times, expected 3"
  if grep -Fx -- "$forbidden" "$log" >/dev/null; then
    fail "$mode writer used incompatible highlighting option: $forbidden"
  fi
}

run_case legacy --no-highlight --syntax-highlighting=none
run_case current --syntax-highlighting=none --no-highlight
