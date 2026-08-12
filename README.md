# libpkgsource-yaml

`libpkgsource-yaml` is the strict, bounded YAML syntax frontend for native
Zeppe-Lin package-source declarations.

The library accepts caller-owned document bytes and returns parser-neutral
`libpkgsource` declarations. It does not open files, discover collections,
combine documents, seal semantic authority, resolve requirements, fetch
sources, execute programs, plan transactions, or store evidence.

## Protocols

The first public release owns two input protocols:

- `zeppe-lin.profiles/1` for named requirement-profile declarations;
- `zeppe-lin.recipe/1` for complete package-source declarations, including an
  optional check program.

The experimental recipe-generation split from the pre-release in-tree parser is
not published as compatibility history.

## Public API

```cpp
#include <libpkgsource-yaml/libpkgsource-yaml.h>

auto declarations = pkgsource::yaml::parse_profiles_yaml(
    profile_bytes, pkgsource::source_origin("profiles.yml"));
auto profiles = pkgsource::profile_catalog::seal(std::move(declarations));

auto declaration = pkgsource::yaml::parse_recipe_yaml(
    recipe_bytes, pkgsource::source_origin("recipe.yml"));
auto snapshot = pkgsource::seal_source(
    pkgsource::source_origin("recipe.yml"),
    std::move(declaration),
    profiles);
```

Parsing and semantic sealing remain explicit, separate calls. Parser failures
are `pkgsource::yaml::yaml_error` values with stable categories, schema paths,
and one-based source locations. Sealing failures remain `pkgsource::error`
values owned by `libpkgsource`.

## Strictness and bounds

The parser rejects duplicate and unknown keys, non-scalar mapping keys,
multiple or empty documents, directives, anchors, aliases, merge keys, custom
or kind-incompatible tags, scalar requirement shorthand, and protocol drift.

Each call is bounded by `parse_limits`: total document bytes, bytes in one
scalar, retained node count, and node depth. All ceilings are inclusive.

## Provider boundary

Grammar and diagnostics are provider-neutral. Only
`src/internal/yaml_event_reader_libyaml.cpp` knows libyaml types and event
layouts. Replacing the event provider must preserve the normalized event and
error contract; changing the accepted YAML subset or document grammar is a
protocol change.

## Documentation

Canonical project documentation is installed under
`share/doc/libpkgsource-yaml`. Generated manual pages are installed under the
normal man hierarchy.

With `-Dhtml_docs=enabled`, the repository also generates a versioned static
artifact under `share/htmldocs/libpkgsource-yaml/1.0.1`. The project repository
owns generation; a website may publish the resulting tree without rebuilding
or reinterpreting it.

See `DESIGN.md`, `docs/protocols/`, and `TESTING.md` for the
normative boundaries and qualification model.
