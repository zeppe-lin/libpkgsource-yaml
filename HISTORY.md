# History

## 1.0.0

First independent YAML frontend release.

- Extracted the YAML implementation, format contracts, manuals, and tests from
  the pre-release `libpkgsource 2.1.0` repository through a reviewable import
  commit.
- Established a parser-only API returning `profile_declaration` and
  `recipe_declaration` values without invoking semantic sealers.
- Published one `zeppe-lin.recipe/1` protocol containing the optional check
  program; the experimental recipe/1 versus recipe/2 split is not retained as
  public compatibility history.
- Added explicit document-byte, scalar-byte, node-count, and nesting-depth
  limits with a stable `resource_limit` failure category.
- Separated parser errors from `libpkgsource` sealing errors and retained exact
  document, schema-path, line, and column provenance.
- Added strict grammar, resource-bound, public-header, metadata, and authority-
  boundary contract tests.
- Exported `libpkgsource` once as a public pkg-config requirement while
  retaining libyaml once in the private static-link closure.
- Published the parser design, protocol specifications, migration contract, and
  testing obligations in this repository.

## Import boundary

The first repository commit imports the YAML component exactly from
`libpkgsource 2.1.0`. It is provenance for review, not a published API or ABI.
