# Maintaining libpkgsource-yaml

## Dependency discipline

Release against a signed compatible `libpkgsource` tag and the qualified YAML
provider version. Public headers may expose `libpkgsource` types but never
provider types. Pkg-config must publish `libpkgsource` once in `Requires` and the
selected provider once in `Requires.private`.

## Protocol discipline

Do not create a new document version to preserve unreleased experiments. A new
version requires a real installed population, a written compatibility policy,
complete protocol specification, migration guidance, and tests for every
accepted and rejected generation.

Provider replacement is not permission to change accepted syntax or diagnostics.
Run the normalized event and full grammar suites against every provider.

## Generated documentation

Markdown under `docs/` is canonical. Regenerate committed roff with
`tools/update-man-pages.sh --write`. HTML is a versioned derived artifact built
from canonical Markdown and installed public headers.

## Release checklist

1. Build shared and static closures with GCC and Clang.
2. Run all tests with warnings as errors.
3. Run GCC and Clang ASan/UBSan jobs.
4. Run clang-format 17, Doxygen, Pandoc regeneration, mandoc lint, and HTML link
   validation.
5. Inspect pkg-config, SONAME, `NEEDED`, and the exact ABI manifest.
6. Test installed shared and static consumers.
7. Stage `doc`, `man`, and `html-docs` installation through `DESTDIR`.
8. Replay the release series independently and compare the final tree.
9. Tag only after all three coordinated source repositories remain green.
