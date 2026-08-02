% PKGSOURCE_RECIPE_YAML(5) libpkgsource-yaml 1.0.0 | libpkgsource-yaml

# NAME

pkgsource_recipe_yaml - describe the native recipe YAML version 1 protocol

# DESCRIPTION

A `zeppe-lin.recipe/1` document declares one package release. Its root contains
required `format`, `package`, `requirements`, `sources`, and `build` values, plus
optional `check`, `lifecycle`, and `architectures` values.

This first published protocol includes the optional check program. There is no
published recipe/2 compatibility generation.

Requirements use explicit `package` or `profile` subject mappings and retain
separate build, run, check, and action-bound lifecycle scopes. Sources contain
exactly one `url` or `path`, plus explicit `name` and lowercase SHA-256 fields.
Programs contain exact `language: posix-shell` and `script` fields.

# AUTHORITY

The parser returns one `recipe_declaration`. It does not seal profiles or source
authority, open files, fetch inputs, execute programs, resolve requirements, or
construct plans.

The caller passes the declaration and one explicit sealed profile catalog to
`seal_source()`. That core operation owns profile expansion, normalization,
duplicate detection, lifecycle and check-program binding, and source identity.

# YAML CONTRACT

Exactly one strict YAML document is accepted. Directives, anchors, aliases,
merge keys, custom or incompatible tags, duplicate keys, complex mapping keys,
unknown schema keys, scalar requirement shorthand, and type drift are rejected.
Parser resource limits apply.

# SEE ALSO

`pkgsource_yaml(3)`, `pkgsource_profiles_yaml(5)`
