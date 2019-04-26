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

        static_assert(matter::detail::tuple_in_list_v<std::tuple<>>);

        static_assert(std::is_same_v<
                      matter::detail::merge_non_void_t<int, float, void, int>,
                      std::tuple<int, float, int>>);

        SECTION("merge tuples")
        {
            static_assert(std::is_same_v<
                          std::tuple<float, int, char>,
                          matter::meta::merge_tuple_types_t<std::tuple<float>,
                                                            std::tuple<int>,
                                                            std::tuple<char>>>);
            static_assert(!std::is_same_v<
                          std::tuple<double, int, char>,
                          matter::meta::merge_tuple_types_t<std::tuple<float>,
                                                            std::tuple<int>,
                                                            std::tuple<char>>>);
        }
    }

    SECTION("find types")
    {
        static_assert(matter::detail::search_type<float, float>::value == 0);
        static_assert(
            matter::detail::search_type<float, int, long, float>::value == 2);
        static_assert(
            matter::detail::search_type<float, int, float, float>::value == 1);

        static_assert(
            !matter::detail::
                 type_index<float, int, short, char, std::tuple<float>>()
                     .has_value());

        static_assert(
            matter::detail::
                type_index<float, int, short, char, float, std::tuple<float>>()
                    .value() == 3);
    }

    SECTION("index range")
    {
        static_assert(std::is_same_v<matter::detail::make_index_range<5, 10>,
                                     std::index_sequence<5, 6, 7, 8, 9>>);
    }
}
