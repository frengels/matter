#include <catch2/catch.hpp>

#include "matter/util/algorithm.hpp"

template<typename T, std::size_t N>
constexpr std::array<T, N> static_insertion_sort(std::array<T, N> arr) noexcept
{
    matter::insertion_sort(arr.begin(), arr.end());
    return arr;
}

TEST_CASE("algorithm")
{
    SECTION("insertion_sort")
    {
        SECTION("constexprness")
        {
            constexpr auto s = static_insertion_sort(std::array{5, 4, 3, 2, 1});
            constexpr auto pres = std::array{1, 2, 3, 4, 5};
            static_assert(matter::equal(s.begin(), s.end(), pres.begin()));
        }
    }
}
