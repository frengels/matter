project(
  'matter',
  'cpp',
  license: 'MIT',
  default_options: [
    'cpp_std=c++17',
    'warning_level=1',
  ]
)

cxx = meson.get_compiler('cpp')

matter_inc = include_directories('include')

catch2_dep = dependency('catch2', fallback: ['catch2', 'catch2_dep'])

subdir('src')

matter_dep = declare_dependency(
  link_with: matter_lib,
  dependencies: matter_deps,
  include_directories: matter_inc,
)

subdir('test')

