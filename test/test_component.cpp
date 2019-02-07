#include <catch2/catch.hpp>

#include "matter/component/identifier.hpp"
#include "matter/component/traits.hpp"
#include "matter/entity/entity.hpp"
#include "matter/entity/entity_manager.hpp"
#include "matter/storage/sparse_vector_storage.hpp"

#include <string>

struct random_component
{
    template<typename Id>
    using storage_type = matter::sparse_vector_storage<Id, random_component>;

    int i;

    random_component(int i) : i{i}
    {}
};

struct empty_component
{};

TEST_CASE("component")
{
    static_assert(
        matter::is_component_storage_defined_v<random_component, uint32_t>);

    static_assert(matter::is_component_empty_v<empty_component>);
    static_assert(
        !matter::is_component_storage_defined_v<empty_component, uint32_t>);
}

template<int>
struct test_tag
{};

TEST_CASE("identifier")
{
    SECTION("decay")
    {
        REQUIRE(matter::identifier<test_tag<0>>::get<float>() ==
                matter::identifier<test_tag<0>>::get<const float&&>());
    }

    SECTION("different tag")
    {
        REQUIRE(matter::identifier<test_tag<1>>::get<float>() ==
                matter::identifier<test_tag<2>>::get<int>());
    }

    SECTION("consecutive ids")
    {
        auto id1 = matter::identifier<test_tag<3>>::get<int>();
        auto id2 = matter::identifier<test_tag<3>>::get<float>();
        auto id3 = matter::identifier<test_tag<3>>::get<uint8_t>();

        REQUIRE(id1 < id2);
        REQUIRE(id2 < id3);
    }
}
