catch2_lib = static_library(
  'catch2',
  'catch2.cpp',
  dependencies: catch2_dep,
)

entity_test = executable(
  'test_entity',
  'test_entity.cpp',
  link_with: catch2_lib,
  dependencies: [catch2_dep, matter_dep],
)

test('entity', entity_test)

component_test = executable(
  'test_component',
  'test_component.cpp',
  link_with: catch2_lib,
  dependencies: [catch2_dep, matter_dep],
)

test('component', component_test)
