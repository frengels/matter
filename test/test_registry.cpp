#include <catch2/catch.hpp>

#include <string>

#include "matter/registry.hpp"
#include "matter/entity/entity.hpp"

TEST_CASE("registry")
{

  matter::cx_registry<matter::default_entity, std::string, int, float> registry;

  SECTION("manages")
  {
    CHECK(registry.manages<std::string>());
    CHECK(registry.manages<int>());
    CHECK(registry.manages<float>());

    CHECK(!registry.manages<size_t>());
  }
}
