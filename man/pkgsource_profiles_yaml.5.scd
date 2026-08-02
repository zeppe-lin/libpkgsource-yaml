pkgsource_profiles_yaml(5)

# NAME

pkgsource_profiles_yaml - native profiles.yml/1 input protocol

# DESCRIPTION

A *zeppe-lin.profiles/1* document contains exactly required *format* and
*profiles* keys.  *profiles* is a mapping and may be empty.

Each profile key omits the leading *@*.  Its value contains exactly one required
non-empty *members* sequence.  Every member is an explicit one-key mapping:

```
- package: gcc
- profile: "@compiler"
```

Scalar shorthand is invalid.  Package and profile subjects are distinct value
domains.

# PARSING AND SEALING

The parser returns *profile_declaration* values with document, schema-path, line,
and column provenance.  It does not require every nested profile to occur in the
same document, because an acquisition layer may aggregate explicit documents.

Only *profile_catalog::seal()* owns duplicate-definition and duplicate-member
rejection, unknown-profile and cycle detection, deterministic expansion,
normalization, and profile identities.

# YAML CONTRACT

Exactly one strict YAML document is accepted.  Directives, anchors, aliases,
merge keys, custom or incompatible tags, duplicate keys, complex mapping keys,
unknown schema keys, and type drift are rejected.  Parser resource limits apply.

# SEE ALSO

*pkgsource_yaml*(3), *pkgsource_recipe_yaml*(5)
