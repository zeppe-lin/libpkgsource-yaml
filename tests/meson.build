# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  foreach test_spec : [
    ['provider-events', 'internal/yaml_event_reader_test.cpp'],
    ['parser-subset', 'parser/subset_test.cpp'],
    ['parser-limits', 'parser/limits_test.cpp'],
    ['profiles-content', 'profiles/content_test.cpp'],
    ['profiles-schema', 'profiles/schema_test.cpp'],
    ['recipe-content', 'recipe/content_test.cpp'],
    ['recipe-schema', 'recipe/schema_test.cpp'],
    ['sealing-integration', 'integration/sealing_test.cpp'],
    ['public-umbrella', 'public/umbrella_header_test.cpp'],
    ['public-component', 'public/parser_header_test.cpp'],
  ]
    test_sources = [test_spec[1]]
    test_dependencies = [libpkgsource_yaml_dep]
    test_include_directories = []
    if test_spec[0] == 'provider-events'
      test_sources += [
        '../src/parser.cpp',
        '../src/internal/yaml_event_reader_libyaml.cpp',
      ]
      test_dependencies = [libpkgsource_dep] + yaml_provider_dependencies
      # This target recompiles parser.cpp outside the library target. Carry the
      # source-tree public include root explicitly so an older installed
      # libpkgsource-yaml header cannot satisfy its angle-bracket include.
      test_include_directories = [public_inc, internal_inc]
    endif
    test_executable = executable(
      test_spec[0] + '-test',
      test_sources,
      include_directories: test_include_directories,
      dependencies: test_dependencies,
      cpp_args: visibility_cpp_args,
      install: false,
    )
    test(test_spec[0], test_executable)
  endforeach

  sh = find_program('sh')
  if library_kind == 'shared'
    nm = find_program('nm')
    test(
      'abi-surface',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_abi_surface.sh'),
        nm.full_path(),
        libpkgsource_yaml.full_path(),
        meson.project_source_root() / 'abi/libpkgsource-yaml.exports',
      ],
    )
  endif

  foreach contract : [
    'provider-boundary',
    'parser-boundary',
    'documentation-contract',
    'doxygen-contract',
    'release-contract',
    'style-contract',
  ]
    test(
      contract,
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_' + contract.replace('-', '_') + '.sh'),
        meson.project_source_root(),
      ],
    )
  endforeach

  foreach contract : [
    'documentation-install-contract',
    'html-docs-contract',
    'manpage-source',
    'manpage-normalizer',
    'manpage-writer-options',
  ]
    test(
      contract,
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_' + contract.replace('-', '_') + '.sh'),
        meson.project_source_root(),
      ],
    )
  endforeach

  pandoc = find_program('pandoc', required: false)
  if pandoc.found()
    test(
      'manpage-generated',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_manpage_generated.sh'),
        meson.project_source_root(),
        pandoc.full_path(),
      ],
    )
  endif

  test(
    'metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/metadata_test.sh'),
      meson.project_build_root(),
      meson.project_version(),
    ],
  )

  clang_format = find_program('clang-format-17', required: false)
  if clang_format.found()
    test(
      'format',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_format.sh'),
        clang_format.full_path(),
        meson.project_source_root(),
      ],
    )
  endif
endif
