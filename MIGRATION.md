# Migration from the in-tree adapter

The YAML component formerly shipped as an optional library inside the
pre-release `libpkgsource` repository. The independent `libpkgsource-yaml 1.0.0`
release intentionally resets that experimental API.

## Build metadata

Consumers now depend directly on:

```text
libpkgsource-yaml >= 1.0.0
libpkgsource >= 3.0.0
```

The installed library is `libpkgsource-yaml.so.1` and the pkg-config module is
`libpkgsource-yaml`.

## Namespace and entry points

Replace `pkgsource::yaml_adapter` with `pkgsource::yaml`.

Replace versioned parsed-document and convenience-sealing calls with the two
parser-only entry points:

```cpp
parse_profiles_yaml(bytes, origin)
parse_recipe_yaml(bytes, origin)
```

The returned values are declarations. Call `profile_catalog::seal()` and
`seal_source()` explicitly in the component that owns aggregation and
composition.

## Recipe protocol

Use:

```yaml
format: zeppe-lin.recipe/1
```

The first published recipe/1 protocol includes the optional root `check`
program. Do not emit `zeppe-lin.recipe/2`; that spelling represented an
unreleased in-repository experiment and has no compatibility status.

## No compatibility layer

There are no aliases for the old namespace, no `_v1` or `_v2` C++ entry points,
no parsed-document wrappers, and no parser convenience functions that seal
source authority. Pre-release consumers must migrate atomically with
`libpkgsource 3.0.0`.
