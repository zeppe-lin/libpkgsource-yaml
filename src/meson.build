# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')

libpkgsource_yaml = library(
  'pkgsource-yaml',
  ['document.cpp', 'profiles.cpp', 'recipe.cpp'],
  include_directories: public_inc,
  dependencies: [libpkgsource_dep, libyaml_dep],
  install: true,
  version: meson.project_version(),
  soversion: '1',
)

libpkgsource_yaml_dep = declare_dependency(
  link_with: libpkgsource_yaml,
  include_directories: public_inc,
  dependencies: [libpkgsource_dep, libyaml_dep],
)
meson.override_dependency('libpkgsource-yaml', libpkgsource_yaml_dep)

install_headers(
  '../include/libpkgsource-yaml/libpkgsource-yaml.h',
  '../include/libpkgsource-yaml/parser.h',
  subdir: 'libpkgsource-yaml',
)

pkgconf.generate(
  libpkgsource_yaml,
  name: 'libpkgsource-yaml',
  filebase: 'libpkgsource-yaml',
  description: 'Strict native YAML syntax adapter for libpkgsource',
  subdirs: '.',
  requires: ['libpkgsource >= 3.0.0'],
  requires_private: ['yaml-0.1 >= 0.2.5'],
)
