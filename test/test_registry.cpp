#include <catch2/catch.hpp>

#include <string>

#include "matter/registry.hpp"
#include "matter/entity/entity.hpp"

TEST_CASE("registry")
{

  matter::cx_registry<matter::default_entity, std::string, int, float> registry;

  SECTION("manages")
  {
    static_assert(registry.manages<std::string>());
    static_assert(registry.manages<int>());
    static_assert(registry.manages<float>());

    static_assert(!registry.manages<size_t>());

    // test the automatically generated variadic version
    static_assert(registry.manages<std::string, int, float>());
  }
}
