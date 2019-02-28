#include <catch2/catch.hpp>

#include "matter/component/identifier.hpp"
#include "matter/component/traits.hpp"
#include "matter/entity/entity.hpp"
#include "matter/entity/entity_manager.hpp"
#include "matter/storage/sparse_vector_storage.hpp"

#include <string>

struct random_component
{
    static constexpr auto name = "random_component";

    template<typename Id>
    using storage_type = matter::sparse_vector_storage<Id, random_component>;

    int i;

    random_component(int i) : i{i}
    {}
};

struct empty_component
{};

struct single_depending_struct
{
    using depends_on = empty_component;
};

struct multi_depending_struct
{
    using depends_on = std::tuple<single_depending_struct, empty_component>;
};

struct variant_group
{};

struct variant1
{
    using variant_of = variant_group;
};

struct variant2
{
    using variant_of = variant_group;
};

struct other_variant
{
    using variant_of = empty_component;
};

TEST_CASE("component")
{
    SECTION("is component")
    {
        static_assert(matter::is_component_v<random_component>);
        static_assert(!matter::is_component_v<std::tuple<random_component>>);
    }
    SECTION("storage type")
    {
        static_assert(
            matter::is_component_storage_defined_v<random_component, uint32_t>);

        static_assert(
            !matter::is_component_storage_defined_v<empty_component, uint32_t>);
    }

    SECTION("empty")
    {
        static_assert(matter::is_component_empty_v<empty_component>);
        static_assert(!matter::is_component_empty_v<random_component>);
    }

    SECTION("depends")
    {
        static_assert(
            matter::is_component_dependent_v<single_depending_struct>);
        static_assert(matter::is_component_dependent_v<multi_depending_struct>);
        static_assert(!matter::is_component_dependent_v<empty_component>);

        // check that single types are correctly converted to tuple
        static_assert(std::is_same_v<std::tuple<empty_component>,
                                     typename matter::component_depends_on<
                                         single_depending_struct>::type>);
        // and tuples remain intact
        static_assert(
            std::is_same_v<std::tuple<single_depending_struct, empty_component>,
                           typename matter::component_depends_on<
                               multi_depending_struct>::type>);

        SECTION("dependencies validation")
        {
            static_assert(
                matter::is_component_depends_present_v<single_depending_struct,
                                                       empty_component>);
            static_assert(
                !matter::is_component_depends_present_v<single_depending_struct,
                                                        multi_depending_struct,
                                                        random_component>);

            static_assert(
                matter::is_component_depends_present_v<multi_depending_struct,
                                                       single_depending_struct,
                                                       random_component,
                                                       empty_component>);
        }
    }

    SECTION("variant")
    {
        static_assert(matter::is_component_variant_v<variant1>);
        static_assert(!matter::is_component_variant_v<variant_group>);

        static_assert(
            matter::is_component_variant_of_v<variant1, variant_group>);
        static_assert(
            !matter::is_component_variant_of_v<single_depending_struct,
                                               variant_group>);

        static_assert(
            std::is_same_v<matter::component_variants_t<variant_group,
                                                        single_depending_struct,
                                                        multi_depending_struct,
                                                        variant1,
                                                        other_variant,
                                                        variant2,
                                                        int>,
                           std::tuple<variant1, variant2>>);
    }

    SECTION("named")
    {
        static_assert(matter::is_component_named_v<random_component>);
        constexpr auto name = matter::component_name_v<random_component>;
        static_assert(name.compare("random_component") == 0);

        static_assert(!matter::is_component_named_v<single_depending_struct>);
    }
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
