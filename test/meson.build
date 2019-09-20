tests = [
  'test_entt',
  'test_registry',
  'test_component',
  'test_storage',
  'test_util',
  'test_erased',
  'test_typed_id',
  'test_algo',
  'test_group_container',
  'test_insert_buffer',
  'test_id_cache',
  'test_query',
  'test_job',
  'test_begin_end',
  'test_soa',
  'test_emplace_back',
  'test_span'
]

catch_lib = static_library(
  'catch2',
  'catch2.cpp',
  dependencies: catch2_dep,
)

foreach t : tests
  test_exe = executable(
    t,
    '@0@.cpp'.format(t),
    link_with: catch_lib,
    dependencies: [matter_dep, catch2_dep],
  )
  test(t, test_exe)
endforeach

if get_option('build_benchmarks')
    subdir('bench')
endif
