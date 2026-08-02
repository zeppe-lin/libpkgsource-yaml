# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  foreach test_spec : [
    ['profiles', 'yaml_profiles_test.cpp'],
    ['recipe', 'yaml_recipe_test.cpp'],
    ['limits', 'limits_test.cpp'],
    ['public-headers', 'yaml_adapter_header_test.cpp'],
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
      join_paths(meson.current_source_dir(), 'yaml_adapter_metadata_test.sh'),
      meson.project_build_root(),
      meson.project_version(),
    ],
  )
  test(
    'parser-boundary',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'check_parser_boundary.sh'),
      meson.project_source_root(),
    ],
  )
endif
