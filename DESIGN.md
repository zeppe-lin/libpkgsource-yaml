# Design

## Purpose

`libpkgsource-yaml` translates one strict YAML document into parser-neutral
`libpkgsource` declarations. It is a syntax adapter, not a source-authority
library.

The repository is separate because YAML grammar, libyaml integration, parser
resource policy, diagnostics, and document-version evolution are independent
from semantic source sealing and from planner projection.

## Authority boundary

The parser owns:

- accepted YAML structure and field spelling;
- mapping from document fields to `libpkgsource` declaration constructors;
- document, schema-path, line, and column diagnostics;
- parser resource ceilings;
- rejection of unsupported YAML features and schema drift.

The parser does not own:

- package, profile, program, source, or architecture semantic validity beyond
  construction of the corresponding core value;
- profile expansion, duplicate semantic declaration detection, cycle checks,
  requirement normalization, lifecycle binding, or source identities;
- file access, collection discovery, precedence, or document aggregation;
- dependency resolution, source acquisition, execution, planning, or storage.

A parsed value remains a declaration. Authority begins only when the caller
passes it to `profile_catalog::seal()` or `seal_source()`.

## API shape

The public API has one unversioned operation per currently published document
kind:

```cpp
std::vector<profile_declaration> parse_profiles_yaml(...);
recipe_declaration parse_recipe_yaml(...);
```

Document protocol versions remain explicit inside the document through the
`format` field. They are not copied into C++ function names. A future protocol
version is admitted only when an installed population requires evolution and
its compatibility policy has been designed; it is not created to preserve
unreleased experiments.

The parser deliberately provides no `parse_and_seal` convenience function.
Keeping the composition visible prevents syntax code from becoming an alternate
semantic authority and lets callers aggregate multiple profile documents before
one explicit sealing operation.

## Error model

`yaml_error` reports parser and schema failures with:

- a stable `yaml_error_code`;
- the caller-supplied document name;
- a schema path;
- a one-based line and column;
- a human-readable diagnostic.

Errors raised while constructing an individual `libpkgsource` semantic value
are translated to `invalid_value` at the exact YAML location. Errors raised by
later profile or source sealing are not translated.

## Resource model

The parser operates on caller-owned in-memory bytes and applies four explicit
ceilings:

- maximum document bytes before libyaml is entered;
- maximum bytes in any scalar event;
- maximum scalar, sequence, and mapping nodes;
- maximum node nesting depth, with the root at depth one.

The defaults are conservative operational bounds, not semantic limits of the
package model. Callers may select smaller or larger positive ceilings for their
acquisition policy. Reaching a ceiling fails closed with `resource_limit`.

The document-byte bound limits total input and therefore bounds scanner input.
The scalar, node, and depth bounds additionally constrain parser amplification
and the library's retained tree.

## Dependency boundary

The shared library has public dependency on the matching `libpkgsource` major
API and private dependency on libyaml. Public headers expose no libyaml types.
The core `libpkgsource` repository has no reverse dependency on this library.
