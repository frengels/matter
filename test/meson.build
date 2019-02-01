tests = [
  'registry',
  'component',
  'ecs',
  'entity',
  'variant',
]

catch_lib = static_library(
  'catch2',
  'catch2.cpp',
  dependencies: catch2_dep,
)

foreach t : tests
  test_exe = executable(
    'test_@0@.cpp'.format(t),
    link_with: catch_lib,
    dependencies: [ matter ],
  )
  test(t, test_exe)
endforeach
