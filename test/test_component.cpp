#include <catch2/catch.hpp>

#include "matter/component/registry.hpp"
#include "matter/component/traits.hpp"
#include "matter/entity/entity.hpp"
#include "matter/entity/entity_manager.hpp"
#include "matter/id/default_component_identifier.hpp"
#include "matter/id/identifier.hpp"
#include "matter/storage/sparse_vector_storage.hpp"

#include <string_view>

struct random_component
{
    static constexpr auto name = "random_component";

    using storage_type =
        matter::sparse_vector_storage<std::size_t, random_component>;

    int i;

    random_component(int i) : i{i}
    {}
};

struct empty_component
{
    static constexpr auto name = "empty_component";
};

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
        static_assert(matter::is_component_storage_defined_v<random_component>);

        static_assert(!matter::is_component_storage_defined_v<empty_component>);
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

    SECTION("component_identifier")
    {
        matter::default_component_identifier<matter::unsigned_id<std::size_t>,
                                             float,
                                             int,
                                             std::string_view,
                                             empty_component>
            cident;

        static_assert(cident.id<float>().value() == 0);
        static_assert(cident.is_static<std::string_view>());
        static_assert(!cident.is_static<std::wstring_view>());
        CHECK(cident.template id<float>().value() == 0);

        static_assert(cident.id<std::string_view>() ==
                      cident.id<std::string_view>());

        // we test whether the global id doesn't affect the local id
        CHECK(!cident.contains<std::wstring_view>()); // this will
                                                      // generate a
                                                      // global id
        cident.register_component<random_component>(); // this will generate a local
                                                  // id of num+0
        cident.register_component<std::wstring_view>();
        CHECK(cident.id<random_component>().value() ==
              decltype(cident)::constexpr_components_size);
        CHECK(cident.id<std::wstring_view>().value() ==
              decltype(cident)::constexpr_components_size + 1);
    }
}

template<int>
struct test_tag
{};

TEST_CASE("identifier")
{
    SECTION("decay")
    {
        REQUIRE(
            matter::identifier<std::size_t, test_tag<0>>::get<float>() ==
            matter::identifier<std::size_t, test_tag<0>>::get<const float&&>());
    }

    SECTION("different tag")
    {
        REQUIRE(matter::identifier<std::size_t, test_tag<1>>::get<float>() ==
                matter::identifier<std::size_t, test_tag<2>>::get<int>());
    }

    SECTION("consecutive ids")
    {
        auto id1 = matter::identifier<int, test_tag<3>>::get<int>();
        auto id2 = matter::identifier<int, test_tag<3>>::get<float>();
        auto id3 = matter::identifier<int, test_tag<3>>::get<uint8_t>();

        REQUIRE(id1 < id2);
        REQUIRE(id2 < id3);
    }
}
