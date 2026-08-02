# Profiles YAML version 1

`zeppe-lin.profiles/1` declares named requirement profiles. The parser returns
`profile_declaration` values; only a later `profile_catalog::seal()` result is
profile authority.

## Root document

```yaml
format: zeppe-lin.profiles/1
profiles:
  compiler:
    members:
      - package: gcc
  toolchain:
    members:
      - profile: "@compiler"
      - package: binutils
      - package: make
```

The root is a mapping containing exactly `format` and `profiles`. Both are
required. `format` is the exact scalar `zeppe-lin.profiles/1`. `profiles` is a
mapping and may be empty.

## Profile declarations

Each key below `profiles` is the profile name without its leading `@`. The
parser constructs `profile_reference("@" + key)`, so core canonical-name rules
apply at that key's source location.

Each profile value is a mapping containing exactly one required `members`
sequence. The sequence must be non-empty according to the core declaration
contract. Each member is exactly one explicit subject mapping:

```yaml
- package: pkg-config
- profile: "@toolchain"
```

A member mapping must contain exactly one of `package` or `profile`. Scalar
shorthand is invalid. A package subject and a profile subject are different
semantic domains.

The parser does not require nested profile references to be defined in the same
individual document. An acquisition layer may aggregate declarations from
multiple explicit documents before one call to `profile_catalog::seal()`.
Unknown profiles, duplicate definitions or members, and cycles are therefore
sealer errors, not document-parser errors.

## YAML subset

Exactly one YAML document is accepted. The protocol rejects:

- YAML directives;
- anchors, aliases, and merge keys;
- custom or node-kind-incompatible tags;
- duplicate mapping keys;
- non-scalar mapping keys;
- unknown schema keys;
- multiple or empty documents.

Standard string, integer, sequence, and mapping tags are accepted only when they
match the actual node kind. YAML implicit typing does not create package
semantics; scalar bytes are passed to core value constructors.

## Provenance and ordering

The parser records the caller-supplied document name, schema path, and one-based
line and column for each declaration and member. These locations are diagnostic
provenance.

Document mapping and member order are retained in declarations. Deterministic
normalization, expansion, and identity computation belong to
`profile_catalog::seal()`.

## Resource limits

The whole document, every scalar, total node count, and nesting depth are
bounded by the caller's `parse_limits`. Exceeding a bound produces
`yaml_error_code::resource_limit` before a declaration is returned.

## Non-goals

This protocol does not define file names on disk, collection discovery,
precedence, aggregation policy, profile sealing, requirement resolution, or
storage.
