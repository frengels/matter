#include <catch2/catch.hpp>

#include <string_view>

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
    std::string_view str;
};

TEST_CASE("registry")
{
    using id_type = matter::unsigned_id<std::size_t>;

    matter::registry<id_type, float_comp, int_comp, string_comp> reg;

    SECTION("group comparison")
    {

        reg.register_component<const char*>();
        reg.register_component<char>();
    }

    SECTION("register")
    {
        reg.register_component<const char*>();
        reg.create<const char*>(std::forward_as_tuple("Hello world"));
    }

    SECTION("create")
    {
        reg.create<float_comp, int_comp>(std::forward_as_tuple(5.0f),
                                         std::forward_as_tuple(5));
    }
}
