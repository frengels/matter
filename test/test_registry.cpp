#include <catch2/catch.hpp>

#include <string>

#include "matter/component/registry.hpp"
#include "matter/component/traits.hpp"
#include "matter/util/erased.hpp"

struct float_comp
{
    float f;
};

struct int_comp
{
    int i;
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
        auto grp = decltype(reg)::group<1>{std::make_pair(
            reg.component_id<int_comp>(),
            matter::make_erased<matter::component_storage_t<int_comp>>())};

        CHECK(grp.contains(reg.component_id<int_comp>()));
        CHECK(!grp.contains(10));
        CHECK(!grp.contains(reg.component_id<float_comp>()));

        auto& vector = grp.get<int_comp>(reg.component_id<int_comp>());

        // do random operations to verify we obtained the correct storage medium
        vector.push_back({5});
        CHECK(vector.size() == 1);
    }
}
