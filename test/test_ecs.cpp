#include <catch2/catch.hpp>

#include "matter/ecs.hpp"

TEST_CASE("ecs")
{
    using entity_type = matter::entity<uint32_t, uint32_t>;
    matter::ecs<entity_type> ecs;

    SECTION("entity creation")
    {
        ecs.create_entity<>();
    }
}
