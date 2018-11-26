cmake_minimum_required(VERSION 3.10)
project(matter LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(matter_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/ecs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/component/component_container.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/component/component_manager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/component/component_traits.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/component/identifier.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/component/sparse_vector.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/entity/entity.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/entity/entity_handle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/entity/entity_manager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/matter/entity/entity_traits.hpp
) 

add_library(matter INTERFACE)
target_sources(matter INTERFACE $<BUILD_INTERFACE:${matter_headers}>)
target_include_directories(matter INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_subdirectory(test)
