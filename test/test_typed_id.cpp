#include <catch2/catch.hpp>

#include "matter/id/default_component_identifier.hpp"
#include "matter/id/typed_id.hpp"

TEST_CASE("typed_id")
{
    SECTION("contains")
    {
        matter::default_component_identifier<matter::signed_id<int>,
                                             char,
                                             int,
                                             float,
                                             short,
                                             long>
            ident{};
        ident.register_component<unsigned char>();
        ident.register_component<double>();
        ident.register_component<unsigned int>();

        SECTION("same size")
        {
            auto unordered1 = ident.component_ids<char, short, float>();
            auto unordered2 = ident.component_ids<short, char, float>();
            auto rt_unorder1 =
                ident.component_ids<unsigned char, double, char>();
            auto rt_unorder2 =
                ident.component_ids<double, unsigned char, char>();

            auto ordered1 = ident.ordered_component_ids<float, short, char>();
            auto rt_order1 =
                ident.ordered_component_ids<char, unsigned char, double>();

            SECTION("get")
            {
                static_assert(!matter::is_typed_id_v<char>);

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
                    CHECK(
                        std::is_same_v<typename std::decay_t<decltype(c)>::type,
                                       char>);
                    CHECK(
                        std::is_same_v<typename std::decay_t<decltype(s)>::type,
                                       short>);
                    CHECK(
                        std::is_same_v<typename std::decay_t<decltype(f)>::type,
                                       float>);
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
            auto unordered1 = ident.component_ids<char, short>();
            auto no_match   = ident.component_ids<int, short, float>();

            auto ordered = ident.ordered_component_ids<char,
                                                       double,
                                                       unsigned int,
                                                       float,
                                                       short>();

            CHECK(ordered.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(!ordered.contains(matter::ordered_typed_ids{no_match}));
        }
    }
}
