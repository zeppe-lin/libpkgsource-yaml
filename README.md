# libpkgsource-yaml

`libpkgsource-yaml` is the strict YAML syntax frontend for native Zeppe-Lin
package-source declarations.

The library accepts caller-owned document bytes and returns parser-neutral
`libpkgsource` declarations. It does not open files, discover collections,
combine profile documents, seal semantic authority, resolve requirements, fetch
sources, or execute programs.

The first public release owns two input protocols:

- `zeppe-lin.profiles/1` for named requirement-profile declarations;
- `zeppe-lin.recipe/1` for complete package-source declarations, including an
  optional check program.

The experimental distinction between recipe versions one and two existed only
inside the pre-release `libpkgsource` repository. It is deliberately not
published here as compatibility history.

## Public boundary

```cpp
#include <libpkgsource-yaml/libpkgsource-yaml.h>

auto declarations = pkgsource::yaml::parse_profiles_yaml(
    bytes, pkgsource::source_origin("profiles.yml"));
auto profiles = pkgsource::profile_catalog::seal(std::move(declarations));

auto declaration = pkgsource::yaml::parse_recipe_yaml(
    recipe_bytes, pkgsource::source_origin("recipe.yml"));
auto snapshot = pkgsource::seal_source(
    pkgsource::source_origin("recipe.yml"),
    std::move(declaration), profiles);
```

Parsing and semantic sealing are intentionally separate calls. Parser failures
are `pkgsource::yaml::yaml_error` values with stable categories and exact
one-based source locations. Sealing failures remain `pkgsource::error` values
owned by `libpkgsource`.

## Strictness and bounds

The parser rejects duplicate and unknown keys, non-scalar mapping keys,
multiple documents, directives, anchors, aliases, merge keys, custom or
kind-incompatible tags, scalar requirement shorthand, and schema drift.

Every call is bounded by `parse_limits`: total input bytes, scalar bytes, parsed
node count, and nesting depth. The library does not expose libyaml types in its
public ABI.

See `docs/architecture.md`, `docs/protocols/profiles-yaml-v1.md`,
`docs/protocols/recipe-yaml-v1.md`, and `docs/testing.md` for the
normative ownership and qualification contracts.
