#include <catch2/catch.hpp>

#include "matter/util/variant.hpp"

TEST_CASE("variant")
{
    matter::variant<8, int, float> my_var(std::in_place_type_t<int>{}, 8);

    CHECK(decltype(my_var)::buffer_size == 8);

    SECTION("is_native")
    {
        CHECK(my_var.is_native<float>());
        CHECK(!my_var.is_native<char>());
    }

    SECTION("get")
    {
        CHECK(my_var.get<int>() == 8);
    }
}
