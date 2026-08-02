# In-tree parser migration

## Origin

The implementation began as the optional `libpkgsource-yaml` target inside the
pre-release `libpkgsource` repository. It was extracted because document syntax,
libyaml integration, parser resource policy, diagnostics, and protocol evolution
have a separate dependency and release lifecycle from semantic source authority.

## Preserved behavior

The extraction preserved the accepted `zeppe-lin.profiles/1` and
`zeppe-lin.recipe/1` grammars, declaration construction, source locations,
strict YAML subset, and explicit parser resource ceilings.

The standalone library continues to return declarations only. Callers retain
visible responsibility for profile aggregation and semantic sealing.

## Removed development history

The in-tree parser briefly distinguished experimental recipe generations before
any package population depended on them. The standalone first release publishes
one `zeppe-lin.recipe/1` grammar containing the current optional check program.
It does not ship an unused compatibility decoder or versioned C++ function
names.

## No compatibility layer

No installed standalone ABI or package population existed before this split.
The initial repository therefore contains no forwarding headers, compatibility
libraries, deprecated entry points, or alternate document generation.
