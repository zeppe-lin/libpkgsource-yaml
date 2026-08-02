// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string_view>

namespace test_documents {

inline constexpr std::string_view profiles = R"(format: zeppe-lin.profiles/1
profiles:
  toolchain:
    members:
      - profile: "@compiler"
      - package: binutils
  compiler:
    members:
      - package: gcc
)";

inline constexpr std::string_view reordered_profiles = R"(profiles:
  compiler:
    members:
      - package: gcc
  toolchain:
    members:
      - package: binutils
      - profile: "@compiler"
format: zeppe-lin.profiles/1
)";

inline constexpr std::string_view complete_recipe = R"(format: zeppe-lin.recipe/1
package:
  name: example
  version: 1.2.3
  release: 1
  summary: Example package
  description: |
    Native example package.
  homepage: https://example.invalid/
  licenses:
    - GPL-3.0-or-later
requirements:
  build:
    - profile: "@toolchain"
    - package: pkg-config
  run:
    - package: libfoo
  check:
    - package: pkgcheck
  lifecycle:
    post-install:
      - package: desktop-file-utils
sources:
  - url: https://example.invalid/example-1.2.3.tar.xz
    name: example-1.2.3.tar.xz
    sha256: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
  - path: files/example.conf
    name: example.conf
    sha256: abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789
build:
  language: posix-shell
  script: |
    meson setup build
    meson compile -C build
    meson install -C build --destdir "$PKG"
check:
  language: posix-shell
  script: |
    meson test -C build
lifecycle:
  post-install:
    language: posix-shell
    script: |
      update-desktop-database
architectures:
  build:
    - x86_64
  target:
    - x86_64
)";

inline constexpr std::string_view reordered_recipe = R"(sources:
  - name: example.conf
    path: files/example.conf
    sha256: abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789
  - sha256: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
    url: https://example.invalid/example-1.2.3.tar.xz
    name: example-1.2.3.tar.xz
format: zeppe-lin.recipe/1
architectures:
  target: [x86_64]
  build: [x86_64]
lifecycle:
  post-install:
    script: |
      update-desktop-database
    language: posix-shell
check:
  script: |
    meson test -C build
  language: posix-shell
build:
  script: |
    meson setup build
    meson compile -C build
    meson install -C build --destdir "$PKG"
  language: posix-shell
requirements:
  check: [{package: pkgcheck}]
  run: [{package: libfoo}]
  lifecycle:
    post-install: [{package: desktop-file-utils}]
  build:
    - package: pkg-config
    - profile: "@toolchain"
package:
  release: 1
  version: 1.2.3
  name: example
  licenses: [GPL-3.0-or-later]
  homepage: https://example.invalid/
  description: |
    Native example package.
  summary: Example package
)";

} // namespace test_documents
