% PKGSOURCE_YAML(3) libpkgsource-yaml 2.0.0 | libpkgsource-yaml

# NAME

pkgsource_yaml - parse strict native YAML into libpkgsource declarations

# SYNOPSIS

```cpp
#include <libpkgsource-yaml/libpkgsource-yaml.h>

std::vector<pkgsource::profile_declaration>
pkgsource::yaml::parse_profiles_yaml(
    std::string_view bytes,
    pkgsource::source_origin origin,
    const pkgsource::yaml::parse_limits& limits = {});

pkgsource::recipe_declaration
pkgsource::yaml::parse_recipe_yaml(
    std::string_view bytes,
    pkgsource::source_origin origin,
    const pkgsource::yaml::parse_limits& limits = {});
```

# DESCRIPTION

`libpkgsource-yaml` parses one caller-owned in-memory YAML document into
parser-neutral `libpkgsource` declarations. It never opens paths or invokes
`profile_catalog::seal()` or `seal_source()`.

The profile parser accepts `zeppe-lin.profiles/1`. The recipe parser accepts
`zeppe-lin.recipe/1`, including its optional check program. Protocol versions
are document fields, not C++ entry-point generations.

# STRICTNESS

The parser rejects multiple or empty documents, directives, duplicate and
unknown keys, non-scalar mapping keys, anchors, aliases, merge keys, custom or
kind-incompatible tags, scalar requirement shorthand, and protocol/type drift.

# LIMITS

`parse_limits` bounds total document bytes, bytes in one scalar, retained nodes,
and nesting depth. The root is depth one. All ceilings are inclusive. Exceeding
one throws `yaml_error` with code `resource_limit`.

# ERRORS

Syntax, unsupported-feature, schema, value, and resource failures throw
`yaml_error`. The value retains a stable `yaml_error_code`, caller-supplied
document label, protocol path, and one-based line and column.

A failure raised while constructing one core semantic value is translated to
`invalid_value` at its YAML location. Later failures from profile or source
sealing remain `pkgsource::error` and are not translated.

# PROVIDER

Only the private libyaml event provider knows libyaml types. Grammar behavior,
diagnostics, and public ABI remain provider-neutral.

# AUTHORITY

Parsed declarations are not package-source authority. Callers explicitly pass
profile declarations to `profile_catalog::seal()` and recipe declarations to
`seal_source()`. Aggregation, precedence, semantic normalization, identity,
resolution, fetching, execution, planning, and storage remain outside this
library.

# ABI

The current public ABI is `libpkgsource-yaml.so.2`. Public headers expose no
libyaml types. Parser results carry `libpkgsource` declarations by value, so the
library requires `libpkgsource >= 4.0.0, < 5.0.0`; libyaml remains private in
pkg-config metadata.

# SEE ALSO

`pkgsource_profiles_yaml(5)`, `pkgsource_recipe_yaml(5)`
