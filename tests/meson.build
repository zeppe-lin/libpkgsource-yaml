# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  foreach test_spec : [
    ['profiles', 'profiles/contract_test.cpp'],
    ['recipe', 'recipe/contract_test.cpp'],
    ['limits', 'parser/limits_test.cpp'],
    ['public-headers', 'public/headers_test.cpp'],
  ]
    test_executable = executable(
      test_spec[0] + '-test',
      test_spec[1],
      dependencies: libpkgsource_yaml_dep,
      install: false,
    )
    test(test_spec[0], test_executable)
  endforeach

  sh = find_program('sh')
  test(
    'metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/metadata_test.sh'),
      meson.project_build_root(),
      meson.project_version(),
    ],
  )
  test(
    'parser-boundary',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_parser_boundary.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'documentation-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_documentation_contract.sh'),
      meson.project_source_root(),
    ],
  )
  test(
    'release-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_release_contract.sh'),
      meson.project_source_root(),
    ],
  )
endif
