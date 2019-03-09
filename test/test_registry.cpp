#include <catch2/catch.hpp>

#include <string>

#include "matter/component/registry.hpp"
#include "matter/component/traits.hpp"
#include "matter/util/erased.hpp"

struct float_comp
{
    float f;

    constexpr float_comp(float f) : f{f}
    {}
};

struct int_comp
{
    int i;

    constexpr int_comp(int i) : i{i}
    {}
};

struct string_comp
{
    std::string str;
};

TEST_CASE("registry")
{
    matter::registry<float_comp, int_comp, string_comp> reg;

    SECTION("group")
    {
        auto grp = decltype(reg)::group<1>{std::forward_as_tuple(
            reg.component_id<int_comp>(),
            std::in_place_type_t<matter::component_storage_t<int_comp>>{})};

        CHECK(grp.contains(reg.component_id<int_comp>()));
        CHECK(!grp.contains(10));
        CHECK(!grp.contains(reg.component_id<float_comp>()));

        auto& vector = grp.get<int_comp>(reg.component_id<int_comp>());

        // do random operations to verify we obtained the correct storage medium
        vector.push_back({5});
        CHECK(vector.size() == 1);
    }

    SECTION("group comparison")
    {
        reg.register_component<std::string>();
        reg.register_component<char>();

        // ids should roughly be 1, 0, 3 -> 0, 1, 3
        auto grp = decltype(reg)::group<3>{
            std::forward_as_tuple(
                reg.component_id<int_comp>(),
                std::in_place_type_t<matter::component_storage_t<int_comp>>{}),
            std::forward_as_tuple(
                reg.component_id<float_comp>(),
                std::in_place_type_t<
                    matter::component_storage_t<float_comp>>{}),
            std::forward_as_tuple(
                reg.component_id<std::string>(),
                std::in_place_type_t<
                    matter::component_storage_t<std::string>>{})};

        SECTION("<")
        {

            // should be 0, 1, 3 < 0, 1, 4
            CHECK(grp < std::array{reg.component_id<float_comp>(),
                                   reg.component_id<int_comp>(),
                                   reg.component_id<char>()});

            // test a smaller array
            CHECK(grp < std::array{reg.component_id<char>()});     // 4
            CHECK(grp < std::array{reg.component_id<float_comp>(), // 0, 4
                                   reg.component_id<char>()});

            // 0, 1, 3 <-> 0, 3
            CHECK(!(grp < std::array{reg.component_id<float_comp>(),
                                     reg.component_id<std::string>()}));
        }

        SECTION("contains")
        {
            // single components
            CHECK(grp.contains(std::array{reg.component_id<float_comp>()}));
            CHECK(grp.contains(std::array{reg.component_id<int_comp>()}));
            CHECK(grp.contains(std::array{reg.component_id<std::string>()}));
        }
    }

    SECTION("register")
    {
        reg.register_component<std::string>();
        reg.create<std::string>(std::forward_as_tuple("Hello world"));
    }

    SECTION("create")
    {
        reg.create<float_comp, int_comp>(std::forward_as_tuple(5.0f),
                                         std::forward_as_tuple(5));
    }
}
