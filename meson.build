project(
  'matter',
  'cpp',
  license: 'MIT',
  meson_version: '>=0.48.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
  ],
)

matter_inc = include_directories('include')

if get_option('build_tests')
    catch2_dep = dependency('catch2', fallback: ['catch2', 'catch2_dep'])
endif

if get_option('build_tests') and get_option('build_benchmarks')
    google_benchmark_dep = dependency('benchmark', fallback: ['google-benchmark', 'google_benchmark_dep'])
endif

matter = declare_dependency(
  include_directories: matter_inc,
)

if get_option('build_tests')
    subdir('test')
endif
