#include <catch2/catch.hpp>

#include <string>

#include "matter/component/group_vector.hpp"
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
        auto grp_vec = matter::group_vector{1};

        auto grp =
            grp_vec.emplace<int_comp>(std::array{reg.component_id<int_comp>()});

        CHECK(grp.contains(reg.component_id<int_comp>()));
        CHECK(!grp.contains(10));
        CHECK(!grp.contains(reg.component_id<float_comp>()));

        auto& vector = grp.storage<int_comp>(reg.component_id<int_comp>());

        // just test whether the retrieve vector is correct
        vector.emplace_back(5);
        CHECK(vector.size() == 1);
        CHECK(vector[0].i == 5);
    }

    SECTION("group comparison")
    {

        reg.register_component<std::string>();
        reg.register_component<char>();

        // ids should roughly be 1, 0, 3 -> 0, 1, 3
        auto grp_vec = matter::group_vector{3};
        auto grp     = grp_vec.emplace<int_comp, float_comp, std::string>(
            std::array{reg.component_id<int_comp>(),
                       reg.component_id<float_comp>(),
                       reg.component_id<std::string>()});

        SECTION("<")
        {

            // should be 0, 1, 3 < 0, 1, 4
            CHECK(grp < std::array{reg.component_id<float_comp>(),
                                   reg.component_id<int_comp>(),
                                   reg.component_id<char>()});
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
