#include <catch2/catch.hpp>

#include "matter/util/meta.hpp"

struct foo
{
    int   i{};
    float f{};
    char  c{};

    constexpr foo(int i = {}, float f = {}, char c = {}) noexcept
        : i{i}, f{f}, c{c}
    {}

    constexpr bool operator==(const foo& other) const noexcept
    {
        return i == other.i && f == other.f && c == other.c;
    }

    constexpr bool operator!=(const foo& other) const noexcept
    {
        return !(*this == other);
    }
};

struct move_incrementer
{
    int* i;

    constexpr move_incrementer(int& i) noexcept : i{std::addressof(i)}
    {}

    constexpr move_incrementer(const move_incrementer&) = delete;
    constexpr move_incrementer& operator=(const move_incrementer&) = delete;

    constexpr move_incrementer(move_incrementer&& other) noexcept : i{other.i}
    {
        ++(*i);
    }
    constexpr move_incrementer& operator=(move_incrementer&& other) noexcept
    {
        i = other.i;
        ++(*i);
        return *this;
    }
};

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

    SECTION("construct_ambiguous")
    {
        auto bar1 = matter::detail::construct_from_ambiguous<foo>(
            std::forward_as_tuple(5, 42.0f, 'f'));

        auto bar2 = matter::detail::construct_from_ambiguous<foo>(bar1);

        CHECK(bar1 == bar2);

        int                           i = 0;
        std::vector<move_incrementer> foo_vec;

        // shows we can perfectly emplace from a tuple
        matter::detail::emplace_back_from_tuple(foo_vec,
                                                std::forward_as_tuple(i));
        CHECK(i == 0);

        int j = 0;

        // perfect emplacement not possible normally
        foo_vec.emplace_back(
            matter::detail::construct_from_ambiguous<move_incrementer>(j));
        CHECK(j == 1);

        int k = 0;
        matter::detail::emplace_back_ambiguous(foo_vec, k);
        CHECK(k == 0);

        int l = 0;
        matter::detail::emplace_back_ambiguous(foo_vec,
                                               std::forward_as_tuple(l));
        CHECK(l == 0);
    }
}
