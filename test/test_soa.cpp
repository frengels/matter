#include <catch2/catch.hpp>

#include <hera/type_identity.hpp>

#include "matter/container/soa.hpp"
#include "matter/iterator/begin.hpp"

template<matter::random_access_iterator I>
constexpr void verify_it(I)
{}

template<matter::movable T>
constexpr void verify_movable(T)
{}

TEST_CASE("soa")
{
    auto s = matter::soa<std::vector<int>, std::vector<float>>{
        std::vector<int>{}, std::vector<float>{}};

    // Does not compile because of the different keys requirement
    // matter::soa<hera::pair<hera::type_identity<int>, std::vector<float>>,
    //             hera::pair<hera::type_identity<int>, std::vector<int>>>{};

    static_assert(decltype(s.has_value_type<float>())::value);
    static_assert(decltype(s.has_value_type<int>())::value);
    static_assert(!decltype(s.has_value_type<char>())::value);

    REQUIRE(s.empty());
    REQUIRE(s.size() == 0);

    s.push_back(5, 5.0f);

    REQUIRE(!s.empty());
    REQUIRE(s.size() == 1);

    // auto it = matter::begin(s);

    SECTION("emplace_back")
    {
        REQUIRE(s.size() == 1);
        s.emplace_back(hera::forward_as_tuple(5),
                       hera::forward_as_tuple(42.0f));
        REQUIRE(s.size() == 2);

        auto [i1, f1] = *matter::begin(s);
        REQUIRE(i1 == 5);
        REQUIRE(f1 == 5.0f);

        SECTION("erase")
        {
            s.erase(matter::begin(s));

            auto [i2, f2] = *matter::begin(s);

            REQUIRE(i2 == 5);
            REQUIRE(f2 == 42.0f);
        }
    }

    SECTION("keys_ordered")
    {

        auto s2 = matter::soa{std::vector<uint8_t>{},
                              std::vector<char>{},
                              std::vector<int>{},
                              std::vector<float>{}};

        s2.push_back(42, 'b', 0, 5.0f);

        auto beg1  = s2.begin();
        auto beg11 = beg1;
        ++beg1;
        ++beg1;
        beg1 = beg11; // test copy assign

        auto tup = hera::tuple{5, 4};
        tup      = hera::tuple{42, 69};

        verify_it(std::vector<int>::iterator{});

        // verify_movable(hera::tuple{1, 1});
        verify_it(beg1);

        auto all_prox      = *beg1;
        auto [u8, c, i, f] = all_prox;
        REQUIRE(u8 == 42);
        REQUIRE(c == 'b');
        REQUIRE(i == 0);
        REQUIRE(f == 5.0f);
        static_assert(std::is_lvalue_reference_v<decltype(u8)>);
        static_assert(std::is_lvalue_reference_v<decltype(c)>);
        static_assert(std::is_lvalue_reference_v<decltype(i)>);
        static_assert(std::is_lvalue_reference_v<decltype(f)>);

        auto beg2   = s2.begin(hera::type_identity<uint8_t>{});
        auto [u8_2] = *beg2;
        static_assert(matter::same_as<uint8_t&, decltype(u8_2)>);

        REQUIRE(u8_2 == 42);

        auto&& [m_u8, m_c, m_i, m_f] = matter::iter_move(beg1);

        static_assert(std::is_rvalue_reference_v<decltype(m_u8)>);
        static_assert(std::is_rvalue_reference_v<decltype(m_c)>);
        static_assert(std::is_rvalue_reference_v<decltype(m_i)>);
        static_assert(std::is_rvalue_reference_v<decltype(m_f)>);

        auto end1 = s2.end();
        REQUIRE(end1 == ++beg1);

        auto end2 = s2.end(hera::type_identity<uint8_t>{});

        REQUIRE(end2 == ++beg2);
    }
}