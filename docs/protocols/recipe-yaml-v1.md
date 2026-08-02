# Recipe YAML version 1

`zeppe-lin.recipe/1` declares one complete native package source. The parser
returns a `recipe_declaration`; only `libpkgsource::seal_source()` with an
explicit sealed profile catalog produces source authority.

This is the first published recipe protocol. It includes an optional check
program. The pre-release distinction between recipe/1 without check execution
and recipe/2 with check execution is intentionally not preserved.

## Root document

The root is a mapping containing only:

- required `format`;
- required `package`;
- required `requirements`;
- required `sources`;
- required `build`;
- optional `check`;
- optional `lifecycle`;
- optional `architectures`.

`format` must be exactly `zeppe-lin.recipe/1`.

## Package

```yaml
package:
  name: example
  version: 1.2.3
  release: 1
  summary: Example package
  description: Optional longer description.
  homepage: https://example.invalid/
  licenses:
    - GPL-3.0-or-later
```

The package mapping allows exactly `name`, `version`, `release`, `summary`,
`description`, `homepage`, and `licenses`.

`name`, `version`, `release`, `summary`, and `licenses` are required.
`description` and `homepage` are optional. `release` is an unsigned canonical
positive decimal integer: no sign, no leading zeroes except the impossible zero
value, and no value above `uint32_t`. Other semantic constraints come from the
corresponding `libpkgsource` value constructors.

## Requirements

```yaml
requirements:
  build:
    - profile: "@toolchain"
    - package: pkg-config
  run:
    - package: libfoo
  check:
    - package: pkgcheck
  lifecycle:
    post-install:
      - package: desktop-file-utils
```

The mapping allows `build`, `run`, `check`, and `lifecycle`; omitted entries are
empty. Each scope sequence contains explicit one-key `{package: NAME}` or
`{profile: "@NAME"}` mappings. Scalar shorthand and version/provider
expressions are not accepted.

`lifecycle` maps only `pre-install`, `post-install`, `pre-remove`, and
`post-remove` to requirement-subject sequences. The parser constructs typed
scope declarations. The core sealer later requires every lifecycle requirement
scope to have the corresponding lifecycle program and every check requirement
to have a check program.

## Sources

```yaml
sources:
  - url: https://example.invalid/example-1.2.3.tar.xz
    name: example-1.2.3.tar.xz
    sha256: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
  - path: files/example.conf
    name: example.conf
    sha256: abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789
```

`sources` is a required sequence and may be empty. Each entry contains exactly
one of `url` or `path`, plus required `name` and `sha256`. No other keys are
accepted.

The parser constructs remote or local `source_input` values. Core constructors
validate local path safety, source names, and an exact lowercase SHA-256 digest.
The parser does not derive names, open paths, fetch bytes, or verify content.
Duplicate source names are detected by semantic sealing.

## Programs

The build program is required. The check program is optional. Lifecycle
programs are optional and keyed by the four exact lifecycle actions.

```yaml
build:
  language: posix-shell
  script: |
    meson setup build
    meson compile -C build

check:
  language: posix-shell
  script: |
    meson test -C build

lifecycle:
  post-install:
    language: posix-shell
    script: |
      update-desktop-database
```

Every program mapping contains exactly `language` and `script`.
`language` must be the exact scalar `posix-shell`. `script` is passed as exact
scalar bytes to the core program constructor. The parser never executes it.

A check program with no check-specific requirements is valid. Check-specific
requirements without a check program are rejected later by the core sealer.

## Architectures

```yaml
architectures:
  build: [x86_64]
  target: [x86_64]
```

The optional mapping allows only `build` and `target`, each a sequence of
architecture scalars. Missing or empty sequences represent no restriction.
Canonical values, duplicate detection, sorting, and semantic interpretation are
owned by `libpkgsource`.

## YAML subset, provenance, and limits

The strict YAML subset and resource ceilings are identical to
`PROFILES-YAML.md`: one document; no directives, anchors, aliases, merges,
custom tags, duplicate keys, complex keys, or unknown fields; bounded total
bytes, scalar bytes, nodes, and depth.

Every constructed declaration retains the caller's document name, schema path,
and one-based line and column. Mapping order and locations are not semantic
source authority. Normalization, profile expansion, lifecycle/check binding,
duplicate detection, and source identity computation belong exclusively to
`seal_source()`.
