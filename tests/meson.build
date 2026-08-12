# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  foreach test_spec : [
    ['provider-events', 'mechanism/yaml_event_reader_test.cpp', 'mechanism'],
    ['parser-subset', 'protocol/yaml_subset_test.cpp', 'protocol'],
    ['parser-limits', 'protocol/resource_limits_test.cpp', 'protocol'],
    ['profiles-content', 'protocol/profiles_content_test.cpp', 'protocol'],
    ['profiles-schema', 'protocol/profiles_schema_test.cpp', 'protocol'],
    ['recipe-content', 'protocol/recipe_content_test.cpp', 'protocol'],
    ['recipe-schema', 'protocol/recipe_schema_test.cpp', 'protocol'],
    ['sealing-integration', 'integration/sealing_test.cpp', 'integration'],
    ['public-umbrella', 'header/umbrella_header_test.cpp', 'header'],
    ['public-component', 'header/parser_header_test.cpp', 'header'],
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
    test(test_spec[0], test_executable, suite: test_spec[2])
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
      suite: 'contracts',
    )
  endif

  foreach contract : [
    ['provider-boundary', 'check_provider_boundary.sh', [meson.project_source_root()]],
    ['provider-failure-contract', 'check_provider_failure_contract.sh', [meson.project_source_root()]],
    ['parser-boundary', 'check_parser_boundary.sh', [meson.project_source_root()]],
    ['test-layout', 'check_test_layout.sh', [meson.project_source_root()]],
    ['documentation-contract', 'check_documentation_contract.sh', [meson.project_source_root()]],
    ['documentation-source', 'check_documentation_source.sh', [meson.project_source_root()]],
    ['repository-contract', 'check_repository_contract.sh', [meson.project_source_root()]],
    ['doxygen-contract', 'check_doxygen_contract.sh', [meson.project_source_root()]],
    ['release-contract', 'check_release_contract.sh', [meson.project_source_root()]],
    ['style-contract', 'check_style_contract.sh', [meson.project_source_root(), meson.project_build_root()]],
    ['documentation-install-contract', 'check_documentation_install_contract.sh', [meson.project_source_root()]],
    ['html-docs-contract', 'check_html_docs_contract.sh', [meson.project_source_root()]],
    ['manpage-source', 'check_manpage_source.sh', [meson.project_source_root()]],
    ['manpage-normalizer', 'check_manpage_normalizer.sh', [meson.project_source_root()]],
    ['manpage-writer-options', 'check_manpage_writer_options.sh', [meson.project_source_root()]],
    ['metadata', 'metadata_test.sh', [meson.project_build_root(), meson.project_version()]],
  ]
    test(
      contract[0],
      sh,
      args: [join_paths(meson.current_source_dir(), 'contracts', contract[1])] + contract[2],
      suite: 'contracts',
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
      suite: 'contracts',
    )
  endif

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
      suite: 'contracts',
    )
  endif
endif
