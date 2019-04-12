#include <catch2/catch.hpp>

#include "matter/component/component_identifier.hpp"
#include "matter/component/typed_id.hpp"

template<typename T, std::size_t Id = 0>
using static_typed_id = matter::static_id<std::size_t, T, Id>;

template<typename T>
using runtime_typed_id = matter::runtime_id<std::size_t, T>;

TEST_CASE("typed_id")
{
    SECTION("basics")
    {
        // do typed_ids get identified correctly
        static_assert(matter::is_typed_id_v<static_typed_id<int>>);
        static_assert(matter::is_typed_id_v<runtime_typed_id<int>>);
    }

    SECTION("contains")
    {
        matter::component_identifier<char, int, float, short, long> ident{};
        ident.register_type<unsigned char>();
        ident.register_type<double>();
        ident.register_type<unsigned int>();

        SECTION("same size")
        {
            auto unordered1  = ident.ids<char, short, float>();
            auto unordered2  = ident.ids<short, char, float>();
            auto rt_unorder1 = ident.ids<unsigned char, double, char>();
            auto rt_unorder2 = ident.ids<double, unsigned char, char>();

            auto ordered1  = ident.ordered_ids<float, short, char>();
            auto rt_order1 = ident.ordered_ids<char, unsigned char, double>();

            SECTION("get")
            {
                static_assert(!matter::is_typed_id_v<char>);
                static_assert(matter::is_typed_id_v<static_typed_id<char>>);

                unordered1.template get<char>();
                static_assert(
                    std::is_same_v<decltype(unordered1.template get<0>()),
                                   decltype(unordered1.template get<char>())>);
                static_assert(
                    std::is_same_v<decltype(unordered2.template get<1>()),
                                   decltype(unordered2.template get<char>())>);
            }

            SECTION("structured bindings")
            {
                SECTION("unordered")
                {
                    auto [c, s, f] = unordered1;
                    CHECK(std::is_same_v<typename decltype(c)::type, char>);
                    CHECK(std::is_same_v<typename decltype(s)::type, short>);
                    CHECK(std::is_same_v<typename decltype(f)::type, float>);
                }

                SECTION("ordered")
                {
                    // 0char, 2float, 3short
                    auto [c, f, s] = ordered1;
                    CHECK(c == ident.id<char>().value());
                    CHECK(f == ident.id<float>().value());
                    CHECK(s == ident.id<short>().value());
                }
            }

            // static types contains
            CHECK(ordered1.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(ordered1.contains(matter::ordered_typed_ids{unordered2}));

            CHECK(!ordered1.contains(matter::ordered_typed_ids{rt_unorder1}));
            CHECK(!ordered1.contains(matter::ordered_typed_ids{rt_unorder2}));

            // runtime types contains
            CHECK(rt_order1.contains(matter::ordered_typed_ids{rt_unorder1}));
            CHECK(rt_order1.contains(matter::ordered_typed_ids{rt_unorder2}));

            CHECK(!rt_order1.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(!rt_order1.contains(matter::ordered_typed_ids{unordered2}));
        }

        SECTION("smaller")
        {
            auto unordered1 = ident.ids<char, short>();
            auto no_match   = ident.ids<int, short, float>();

            auto ordered =
                ident.ordered_ids<char, double, unsigned int, float, short>();

            CHECK(ordered.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(!ordered.contains(matter::ordered_typed_ids{no_match}));
        }
    }
}
