#include <catch2/catch.hpp>

#include "matter/util/meta.hpp"

TEST_CASE("util")
{
    SECTION("meta")
    {
        static_assert(matter::detail::type_in_list_v<int, float, double, int>);
        static_assert(
            !matter::detail::type_in_list_v<char, float, double, int>);

        static_assert(
            matter::detail::tuple_in_list_v<std::tuple<int, int>, int>);
        static_assert(!matter::detail::tuple_in_list_v<std::tuple<float, int>,
                                                       float,
                                                       char,
                                                       unsigned char>);
        static_assert(
            matter::detail::tuple_in_list_v<std::tuple<unsigned char, uint32_t>,
                                            float,
                                            char,
                                            unsigned char,
                                            uint16_t,
                                            uint32_t>);
    }
}
