# Architecture

## Authority boundary

`libpkgsource-yaml` owns document syntax, bounded parsing, schema diagnostics,
and translation into parser-neutral `libpkgsource` declarations.

`libpkgsource` owns semantic value domains, profile sealing, source sealing,
normalization, and identities. The parser never becomes a second semantic
authority.

```text
caller-owned YAML bytes
          |
          | libpkgsource-yaml
          v
parser-neutral declarations
          |
          | explicit caller composition
          v
libpkgsource sealing authority
```

## Parser pipeline

One parse call proceeds through four explicit layers:

1. the private libyaml provider normalizes native events into project-owned
   event kinds, marks, scalar bytes, tags, anchors, and directive presence;
2. the document layer constructs a bounded scalar/sequence/mapping tree and
   rejects excluded YAML features;
3. the profile or recipe grammar admits exact keys and translates fields into
   `libpkgsource` declaration constructors;
4. the public entry point returns declarations without sealing them.

The provider does not know profile or recipe keys. The grammar does not know
`yaml_parser_t`, `yaml_event_t`, or provider enum values.

## Provider boundary

The current qualified provider is libyaml. Only
`src/internal/yaml_event_reader_libyaml.cpp` includes `<yaml.h>`. The provider
seam is internal; no one-choice build option is exposed.

Introduce provider selection only after a second implementation preserves the
complete normalized behavior: event order, scalar bytes, tags, directive
reporting, source marks, syntax diagnostics, and resource-failure placement. A
provider that accepts a different YAML subset requires protocol review rather
than a build-option substitution.

## Grammar ownership

`profiles.cpp` owns the top-level profile-document grammar.

`recipe.cpp` owns top-level recipe assembly. Field-specific recipe decoding is
isolated under `src/internal/recipe_fields.cpp` so package metadata,
requirements, source locators, programs, lifecycle actions, and architectures
can be reviewed independently from orchestration.

Generic path construction, mapping-key checks, node-kind checks, provenance,
and translation of individual `libpkgsource` constructor failures live in the
document layer.

## Error model

`yaml_error` carries:

- a stable `yaml_error_code`;
- the caller-supplied document label;
- a protocol path;
- a one-based line and column;
- human-readable diagnostic text.

The `what()` message is not a stable machine interface. Parser code translates
only `pkgsource::error` raised while constructing the specific declaration value
at one YAML location. Allocation failures, logic failures, and errors from later
semantic sealing retain their original types.

## Resource model

The parser operates on caller-owned in-memory bytes and applies four inclusive
ceilings:

- total document bytes before the provider is entered;
- bytes retained from one scalar event;
- scalar, sequence, and mapping nodes retained in the document tree;
- nesting depth, counting the root as depth one.

The defaults are operational bounds, not semantic maxima of the package model.
Callers may choose any ceilings required by their acquisition policy. A zero
ceiling is valid and excludes every non-empty use of that resource.

## Repository layout

```text
include/libpkgsource-yaml/   installed public API
src/                        public parser entry points
src/internal/               provider, document tree, and grammar helpers
abi/                        reviewed ELF export manifest
docs/protocols/             document protocol specifications
docs/man/                   canonical manual sources and generated roff
tests/parser/               provider-neutral YAML subset and limits
tests/profiles/             profile grammar contracts
tests/recipe/               recipe grammar contracts
tests/integration/          explicit semantic-sealing handoff
tests/internal/             provider normalization contracts
tests/contracts/            repository, ABI, metadata, and documentation gates
tools/                      deterministic generation and installation tools
```

## Installed documentation

Canonical Markdown, protocol specifications, legal files, and manual sources
are installed under `share/doc/libpkgsource-yaml`. Generated roff is installed
under `share/man`.

The source-document installation list is explicit. A new file under `docs/`
does not become a published contract accidentally.

## HTML publication boundary

With `html_docs` enabled, Pandoc renders repository documentation and Doxygen
renders the installed public API into a versioned static tree. The repository
checks local links and rejects escaped source/build paths before installation.

The website consumes that generated tree as an opaque artifact. It does not
rerun the parser documentation toolchain or become another documentation
source of truth.

## Excluded authority

The library does not own file access, collection discovery, profile aggregation,
semantic sealing, source identities, dependency resolution, acquisition,
execution, planning, transaction construction, or evidence storage.
