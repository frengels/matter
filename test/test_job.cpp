#include <catch2/catch.hpp>

#include "matter/system/job.hpp"
#include "matter/system/world_compiler.hpp"
#include "matter/world.hpp"

#include "matter/query/entities.hpp"

TEST_CASE("job")
{
    using boost::hana::type_c;

    auto world = matter::world{};
    auto comp  = matter::world_compiler{
        world, type_c<matter::entities<matter::write<int>>>};

    world.create_entity<int>(1);
    world.create_entity<int>(2);
    world.create_entity<int>(3);
    world.create_entity<int>(4);
    world.create_entity<int>(5);

    (void) comp;

    int count = 0;

    auto j = matter::make_job<matter::entities<matter::write<int>>>(
        [&](auto&& i_groups) {
            for (auto [i_store] : i_groups)
            {
                for (auto i : i_store)
                {
                    REQUIRE(i == ++count);
                }
            }
        });

    matter::invoke_job(j, comp);

    // should be a valid job for this compiled world
    static_assert(matter::is_job_for_world_v<decltype(j), decltype(comp)>);
    static_assert(matter::is_job_for_world_v<decltype(j), decltype(world)>);
    // should also be a valid job for the non compiled world

    REQUIRE(count == 5);
}
