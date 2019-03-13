tests = [
  'registry',
  'component',
  'storage',
  'ecs',
  'entity',
  'variant',
  'util',
  'erased',
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
    dependencies: [matter, catch2_dep],
  )
  test(t, test_exe)
endforeach

subdir('bench')
