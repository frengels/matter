project(
  'matter',
  'cpp',
  license: 'MIT',
  meson_version: '>=0.48.0',
  default_options: [
    'cpp_std=c++2a',
    'warning_level=3',
  ],
)

matter_inc = include_directories('include')

hana_proj = subproject('hana')

range_v3_dep = dependency('range', fallback: ['range-v3', 'range_dep'])
hana_dep = hana_proj.get_variable('hana_dep')
hera_dep = dependency('hera', method: 'cmake', modules: ['hera::hera'])
nameof_dep = dependency('nameof', method: 'cmake', modules: ['nameof::nameof'])


if get_option('build_tests')
    catch2_dep = dependency('catch2', fallback: ['catch2', 'catch2_dep'])
endif

if get_option('build_tests') and get_option('build_benchmarks')
    google_benchmark_dep = dependency('benchmark', fallback: ['google-benchmark', 'google_benchmark_dep'])
endif

matter_dep = declare_dependency(
  include_directories: matter_inc,
  dependencies: [range_v3_dep, hana_dep, hera_dep, nameof_dep],
)

if get_option('build_tests')
    subdir('test')
endif
