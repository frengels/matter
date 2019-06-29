tests = [
  'entt',
  'registry',
  'component',
  'storage',
  'entity',
  'util',
  'erased',
  'typed_id',
  'algo',
  'group_container',
  'insert_buffer',
  'id_cache',
  'query',
]

catch_lib = static_library(
  'catch2',
  'catch2.cpp',
  dependencies: catch2_dep,
)

foreach t : tests
  test_exe = executable(
    t,
    'test_@0@.cpp'.format(t),
    link_with: catch_lib,
    dependencies: [matter_dep, catch2_dep],
  )
  test(t, test_exe)
endforeach

if get_option('build_benchmarks')
    subdir('bench')
endif
