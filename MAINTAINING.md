<!-- SPDX-FileCopyrightText: 2026 Alexandr Savca -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Maintaining

Release only against a signed compatible `libpkgsource` tag. The parser library
has its own project version and SONAME; it does not advance merely because the
core releases.

Before tagging, qualify GCC and Clang, shared and static linkage, parser limits,
malformed-input coverage, generated metadata, installed consumers, manual pages,
and exact patch replay. Confirm that the public library links libyaml privately
and that no sealing symbol is exported or referenced.

`PROFILES-YAML.md` and `RECIPE-YAML.md` are normative grammar documents. Keep
them synchronized with parser behavior and corpus tests. Diagnostics may retain
document coordinates, but spelling and location never become source semantic
identity.
