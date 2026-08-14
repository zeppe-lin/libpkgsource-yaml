# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')
internal_inc = include_directories('.')

# The current provider is an internal implementation choice. Introduce a
# build option only after a second provider has passed the same normalization
# and grammar contracts; a one-choice option would expose configuration without
# offering a real choice.
yaml_provider_sources = files('internal/yaml_event_reader_libyaml.cpp')
yaml_provider_dependencies = [libyaml_dep]

elf_export_generator = find_program(
  meson.project_source_root() / 'tools/generate-elf-export-script.sh',
)
elf_export_script_name = 'libpkgsource-yaml.exports.map'
elf_export_script_path = meson.current_build_dir() / elf_export_script_name
elf_export_script = configure_file(
  input: '../abi/libpkgsource-yaml.exports',
  output: elf_export_script_name,
  command: [elf_export_generator, '@INPUT@'],
  capture: true,
)

library_link_args = [
  '-Wl,--version-script=' + elf_export_script_path,
]
library_link_depends = [elf_export_script]

libpkgsource_yaml = library(
  'pkgsource-yaml',
  files(
    'parser.cpp',
    'profiles.cpp',
    'recipe.cpp',
    'internal/document.cpp',
    'internal/recipe_fields.cpp',
  ) + yaml_provider_sources,
  include_directories: [public_inc, internal_inc],
  dependencies: [libpkgsource_dep] + yaml_provider_dependencies,
  cpp_args: visibility_cpp_args + ['-DPKGSOURCE_YAML_BUILDING_LIBRARY'],
  gnu_symbol_visibility: 'hidden',
  link_args: library_link_args,
  link_depends: library_link_depends,
  install: true,
  version: meson.project_version(),
  soversion: '2',
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
  '../include/libpkgsource-yaml/export.h',
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
