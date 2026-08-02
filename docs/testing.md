# Testing

## Provider behavior

`provider-events` binds libyaml event normalization without importing libyaml
types into the grammar. It checks event ordering, one-based marks, scalar bytes,
and scalar-limit enforcement.

`provider-boundary` proves that libyaml headers, types, and functions appear only
in the libyaml provider translation unit.

## YAML subset behavior

`parser-subset` covers duplicate keys, unknown keys, directives, anchors,
aliases, merge keys, custom tags, multiple and empty documents, complex mapping
keys, and provider syntax failures.

`parser-limits` varies document bytes, scalar bytes, node count, and depth
independently. Each test proves rejection beyond a ceiling and acceptance at the
exact ceiling.

## Profile grammar behavior

`profiles-content` checks declaration material, exact provenance, caller-visible
document order, and integration with profile sealing.

`profiles-schema` checks the protocol version, root type, profile-name values,
non-empty declarations, and explicit member subjects.

## Recipe grammar behavior

`recipe-content` checks every major declaration group, exact program bytes,
optional check execution, and integration with source sealing. Reordered YAML is
sealed to the same source identity.

`recipe-schema` checks protocol version, required and unknown keys, canonical
release integers, explicit requirement subjects, lifecycle action names, and
program language.

`sealing-integration` proves which invariants remain owned by `libpkgsource`:
lifecycle requirement/program binding, check requirement/program binding, and
duplicate source names.

## Public and ABI behavior

Umbrella and component headers compile independently. Generated pkg-config
metadata exposes `libpkgsource` publicly once and libyaml privately once.

The shared-library ABI test compares every dynamic symbol with
`abi/libpkgsource-yaml.exports`. Private parser, provider, standard-library, and
template symbols must not escape.

## Documentation behavior

Contracts enforce ATX Markdown, the restricted man-page profile, generated-roff
drift checks, installed source documentation, generated HTML structure, local
links, and `DESTDIR` staging.

## Release qualification

Before tagging, run clean shared and static builds with GCC and Clang, warnings
as errors, and the complete test suite. Run ASan and UBSan over functional and
provider tests. Run Doxygen with warnings as errors, regenerate manuals with a
qualified Pandoc 3.x writer, lint roff with mandoc, and validate the versioned
HTML tree.

Inspect shared `SONAME` and `NEEDED` entries. Compile installed shared and static
consumers through pkg-config. Replay the patch series independently and compare
the resulting tree.
