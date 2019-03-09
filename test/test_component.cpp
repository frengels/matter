#include <catch2/catch.hpp>

#include "matter/component/component_identifier.hpp"
#include "matter/component/identifier.hpp"
#include "matter/component/registry.hpp"
#include "matter/component/traits.hpp"
#include "matter/entity/entity.hpp"
#include "matter/entity/entity_manager.hpp"
#include "matter/storage/sparse_vector_storage.hpp"

#include <string>

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
        matter::component_identifier<float, int, std::string> cident;

        static_assert(cident.id_constexpr<float>() == 0);
        static_assert(cident.is_constexpr<std::string>());
        static_assert(!cident.is_constexpr<std::string_view>());
        CHECK(cident.template id<float>() == 0);

        // we test whether the global id doesn't affect the local id
        CHECK(!cident.is_registered<std::string_view>()); // this will generate
                                                          // a global id
        cident.register_type<random_component>(); // this will generate a local
                                                  // id of num+0
        cident.register_type<std::string_view>();
        CHECK(cident.id<random_component>() ==
              decltype(cident)::constexpr_components_size);
        CHECK(cident.id<std::string_view>() ==
              decltype(cident)::constexpr_components_size + 1);

        SECTION("metadata")
        {
            auto id = cident.id<random_component>();

            const auto& metadata1 = cident.metadata(id);
            CHECK(metadata1.name.has_value());

            CHECK(metadata1.name->compare(
                      matter::component_name_v<random_component>) == 0);
            CHECK(metadata1.size == sizeof(random_component));
            CHECK(metadata1.align == alignof(random_component));

            // next has no name
            id = cident.id<std::string_view>();

            const auto& metadata2 = cident.metadata(id);

            CHECK(!metadata2.name.has_value());
            CHECK(metadata2.size == sizeof(std::string_view));
            CHECK(metadata2.align == alignof(std::string_view));
        }
    }

    SECTION("registry")
    {
        matter::registry<float, int, char> reg;
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
