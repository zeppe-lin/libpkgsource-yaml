# Contributing to libpkgsource-yaml

## Boundary first

Changes must preserve the parser-only ownership boundary. Implementation code
must not open files, aggregate collections, seal profiles or sources, resolve
requirements, execute programs, construct plans, or define durable evidence.

A grammar change must identify the exact protocol field, diagnostic path,
resource effect, semantic constructor, tests, manual text, protocol document,
and compatibility decision.

## Engineering standard

Follow `docs/code-style.md`. C++ is formatted with clang-format 17. Control-flow
bodies always use braces. Public headers require complete Doxygen. Implementation
comments explain parser invariants and provider boundaries rather than narrating
statements.

Libyaml types and functions belong only in the libyaml provider translation
unit. Generic document-tree and grammar code consume project-owned event types.

## Tests and documentation

Add focused tests under the contract they exercise. Vary one accepted field,
rejected feature, resource ceiling, or semantic handoff at a time. Do not hide
multiple parser rules behind one monolithic fixture.

Update canonical Markdown and generated roff together. Generated files are never
edited directly. Documentation must distinguish syntax behavior, core semantic
behavior, provider implementation, and excluded authority.

## Patch discipline

Keep tree movement, semantic changes, tests, generated material, and repository
policy separate when practical. Every patch must pass `git diff --check` and be
reviewable without reconstructing unrelated intent.
