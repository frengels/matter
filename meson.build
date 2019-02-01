project(
  'matter',
  'cpp',
  license: 'MIT',
  meson_version: '>=0.48.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
  ],
)

matter_inc = include_directories('include')

catch2_subproj = subproject('catch2')
catch2_dep = catch2_subproj.get_variable('catch2_dep')

matter = declare_dependency(
  include_directories: matter_inc,
)

subdir('test')
