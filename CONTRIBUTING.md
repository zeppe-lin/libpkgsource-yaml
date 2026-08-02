<!-- SPDX-FileCopyrightText: 2026 Alexandr Savca -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Contributing

This repository owns YAML grammar and diagnostics only. It may construct
`libpkgsource` declarations; it must not seal profiles or sources, discover
collections, open package trees, resolve dependencies, or define semantic
identity.

Grammar changes require strict positive and negative corpus tests, duplicate and
unknown-key tests, diagnostic provenance tests, and explicit byte, scalar, node,
and nesting-limit coverage. A C++ API change, YAML protocol change, and resource
limit change are separate review decisions even when implemented together.

There is one public `zeppe-lin.recipe/1` grammar. Do not add a new format number
for an unshipped experiment. When deployed compatibility eventually requires a
new grammar, document the old consumer population and maintain both protocols
explicitly.

Run the complete Meson suite in shared and static configurations, compile the
public header independently, inspect generated pkg-config metadata, and run
ASan/UBSan before submission.
