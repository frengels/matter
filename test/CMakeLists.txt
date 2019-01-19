set(catch2_INCLUDE_DIR
  ${CMAKE_SOURCE_DIR}/external)

set(unit_tests
  registry
  component
  ecs
  entity
  variant
  )

function(set_test_source variable test)
  set(${variable} test_${test}.cpp PARENT_SCOPE)
endfunction()

add_library(catch2pc STATIC catch2.cpp)
target_include_directories(catch2pc PRIVATE ${catch2_INCLUDE_DIR})

foreach(name IN LISTS unit_tests)
  set_test_source(test_SRC ${name})
  add_executable(test_${name} ${test_SRC})
  target_include_directories(test_${name} PRIVATE ${catch2_INCLUDE_DIR})
  target_link_libraries(test_${name} matter catch2pc)
  set_property(TARGET test_${name} PROPERTY CXX_STANDARD 17)
  add_test(NAME ${name} COMMAND test_${name})
endforeach()
