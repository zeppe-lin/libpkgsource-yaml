# Testing

The suite qualifies the parser as an independent syntax boundary.

## Functional tests

`profiles` proves:

- complete declaration construction and exact diagnostic provenance;
- order-independent semantic sealing when the declarations are passed to
  `libpkgsource`;
- duplicate keys, unknown keys, directives, anchors, aliases, merge keys,
  custom tags, multiple documents, complex mapping keys, empty documents, type
  drift, format drift, and invalid semantic values are rejected.

`recipe` proves:

- complete recipe/1 construction, including build, check, lifecycle, source,
  architecture, metadata, and requirement declarations;
- optional check programs and exact program bytes;
- equivalent YAML ordering produces equivalent sealed source identity;
- schema failures are parser errors while lifecycle/check/source-set invariants
  remain core sealing errors.

`limits` proves independent document-byte, scalar-byte, node-count, and depth
ceilings and acceptance at an exact configured boundary.

## Boundary tests

`parser-boundary` statically rejects calls from this repository into
`seal_source()`, `seal_recipe()`, or `profile_catalog::seal()`. It also rejects
experimental versioned API names and removed syntax-authority types.

`public-headers` compiles the installed umbrella header without private parser
or libyaml headers.

`metadata` validates project version, SONAME-facing link name, one public
`libpkgsource >= 3.0.0` requirement, and one private `yaml-0.1 >= 0.2.5`
requirement.

## Required release matrix

Before release, run clean shared and static builds with GCC and Clang, warnings
as errors, and all tests enabled. Run ASan and UBSan over the functional suite.
Render the scdoc manuals and lint them with mandoc. Inspect shared-library
`SONAME` and `NEEDED` entries and compile an installed external consumer using
pkg-config.

The static package must carry the complete private libyaml closure through
`pkg-config --static` without exposing libyaml types in public headers.
