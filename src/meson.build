# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')
internal_inc = include_directories('.')

yaml_provider = get_option('yaml_provider')
yaml_provider_sources = []
yaml_provider_dependencies = []

if yaml_provider == 'libyaml'
  yaml_provider_sources += files('internal/yaml_event_reader_libyaml.cpp')
  yaml_provider_dependencies += [libyaml_dep]
else
  error('unsupported YAML provider: ' + yaml_provider)
endif

libpkgsource_yaml = library(
  'pkgsource-yaml',
  files(
    'parser.cpp',
    'profiles.cpp',
    'recipe.cpp',
    'internal/document.cpp',
  ) + yaml_provider_sources,
  include_directories: [public_inc, internal_inc],
  dependencies: [libpkgsource_dep] + yaml_provider_dependencies,
  install: true,
  version: meson.project_version(),
  soversion: '1',
)

libpkgsource_yaml_dep = declare_dependency(
  link_with: libpkgsource_yaml,
  include_directories: public_inc,
  dependencies: [libpkgsource_dep],
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
  # The target contributes its full closure as private requirements. Promote
  # the public semantic owner with its dependency object so Meson de-duplicates
  # it from that closure. The selected YAML provider remains private.
  requires: [libpkgsource_dep],
)
