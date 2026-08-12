# libpkgsource-yaml history

## 1.0.1

Testing-foundation hardening release.

- Preserved provider allocation failures as `std::bad_alloc` instead of
  misreporting them as malformed YAML.
- Kept unsupported YAML features outside the retained-node resource budget.
- Validated dynamic profile names before descendant provenance paths can reuse
  them, preventing core provenance failures from escaping the YAML adapter.
- Reorganized tests by mechanism, protocol, integration, public-header,
  installed-consumer, support, and repository-contract ownership.
- Made the installed consumer execute the parser so shared/static pkg-config
  qualification proves both the semantic-owner and private libyaml closure.
- Pinned hosted provider qualification to the exact upstream libyaml 0.2.5
  release commit.

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
- Isolated libyaml event acquisition behind a provider-neutral internal API
  without exposing a one-choice provider option.
- Separated document-tree handling, recipe field decoding, and public grammar
  orchestration.
- Added explicit public symbol visibility and a reviewed ELF ABI manifest.
- Reorganized tests by provider normalization, strict YAML subset, resource
  limits, profile grammar, recipe grammar, semantic handoff, and repository
  contract.
- Exported `libpkgsource` once as a public pkg-config requirement while
  retaining libyaml once in the private static-link closure.
- Standardized code style, public Doxygen, canonical Markdown manuals,
  installed documentation, and versioned HTML generation.
- Published architecture, protocol, migration, testing, ABI, maintenance, and
  contribution contracts in this repository.

## Import boundary

The first repository commit imports the YAML component exactly from
`libpkgsource 2.1.0`. It is provenance for review, not a published API or ABI.
