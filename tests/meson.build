# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').enabled()
  profiles_test = executable(
    'profiles-test',
    'yaml_profiles_test.cpp',
    dependencies: libpkgsource_yaml_dep,
    install: false,
  )
  test('profiles', profiles_test)

  recipe_test = executable(
    'recipe-test',
    'yaml_recipe_test.cpp',
    dependencies: libpkgsource_yaml_dep,
    install: false,
  )
  test('recipe', recipe_test)

  public_headers = executable(
    'public-headers',
    'yaml_adapter_header_test.cpp',
    dependencies: libpkgsource_yaml_dep,
    install: false,
  )
  test('public-headers', public_headers)

  sh = find_program('sh')
  test(
    'metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'yaml_adapter_metadata_test.sh'),
      meson.project_build_root(),
      '2.1.0',
    ],
  )
endif
