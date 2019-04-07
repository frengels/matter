#include <catch2/catch.hpp>

#include "matter/component/component_identifier.hpp"
#include "matter/component/group.hpp"
#include "matter/component/group_vector_view.hpp"
#include "matter/component/group_view.hpp"
#include "matter/component/registry.hpp"

TEST_CASE("group_vector")
{
    matter::component_identifier<int, float, short, char> ident;

    matter::group_vector grpvec1{1};
    matter::group_vector grpvec2{2};
    matter::group_vector grpvec3{3};

    CHECK(grpvec1.group_size() == 1);
    CHECK(grpvec2.group_size() == 2);
    CHECK(grpvec3.group_size() == 3);

    SECTION("default iterators")
    {
        SECTION("group_view")
        {
            // default constructed iterator and sentinel should both be equal
            matter::group_view<int>::iterator it{};
            matter::group_view<int>::sentinel sent{};

            CHECK(it == sent);
        }

        SECTION("group_vector_view")
        {
            using group_vector_view_type = decltype(matter::group_vector_view{
                ident.ids<int, float>(),
                std::declval<matter::group_vector&>()});

            group_vector_view_type::iterator it{};
            group_vector_view_type::sentinel sent{};

            CHECK(it == sent);
        }
    }

    SECTION("emplace storage")
    {

        grpvec1.emplace(ident.ids<float>());
        grpvec1.emplace(ident.ids<char>());
        grpvec1.emplace(ident.ids<int>());
        grpvec1.emplace(ident.ids<short>());

        grpvec2.emplace(ident.ids<char, short>());
        grpvec2.emplace(ident.ids<int, float>());

        grpvec3.emplace(ident.ids<short, char, float>()); // 1 2 3 -> 3rd
        grpvec3.emplace(ident.ids<int, short, char>());   // 0 2 3 -> 2nd
        grpvec3.emplace(ident.ids<float, int, short>());  // 0 1 3 -> 1st

        SECTION("check empty views")
        {
            ident.register_type<uint64_t>();
            SECTION("size 1")
            {
                auto view =
                    matter::group_vector_view{ident.ids<uint64_t>(), grpvec1};

                auto it  = view.begin();
                auto end = view.end();

                CHECK(it == end);

                SECTION("reverse")
                {
                    auto rit  = view.rbegin();
                    auto rend = view.rend();

                    CHECK(rit == rend);
                }
            }

            SECTION("size 2")
            {
                SECTION("wrong size")
                {
                    auto view = matter::group_vector_view{ident.ids<uint64_t>(),
                                                          grpvec2};

                    auto it  = view.begin();
                    auto end = view.end();

                    CHECK(it == end);

                    SECTION("reverse")
                    {
                        auto rit  = view.rbegin();
                        auto rend = view.rend();

                        CHECK(rit == rend);
                    }
                }

                auto view = matter::group_vector_view{
                    ident.ids<uint64_t, float>(), grpvec2};

                auto it  = view.begin();
                auto end = view.end();

                CHECK(it == end);

                SECTION("reverse")
                {
                    auto rit  = view.rbegin();
                    auto rend = view.rend();

                    CHECK(rit == rend);
                }
            }
        }

        SECTION("sorted")
        {
            SECTION("group_vector size 1")
            {
                auto start = grpvec1.begin();
                auto end   = grpvec1.end();
                CHECK((end - start) == 4);

                // all the emplaced stores should be correctly sorted now
                auto it = grpvec1.begin();
                CHECK((*it).contains(ident.id<int>()));
                ++it;
                CHECK((*it).contains(ident.id<float>()));
                ++it;
                CHECK((*it).contains(ident.id<short>()));
                ++it;
                CHECK((*it).contains(ident.id<char>()));
                ++it;
                CHECK(it == grpvec1.end());
            }

            SECTION("group_vector size 2")
            {
                // check for sizedrange
                auto start = grpvec2.begin();
                auto end   = grpvec2.end();
                CHECK((end - start) == 2);

                auto                    it  = grpvec2.begin();
                matter::const_any_group grp = *it;
                CHECK(grp == ident.ordered_ids<float, int>());
                ++it;
                grp = matter::const_any_group{*it};
                CHECK(grp == ident.ordered_ids<short, char>());
                ++it;
                CHECK(it == grpvec2.end());
            }

            SECTION("group_vector size 3")
            {
                auto start = grpvec3.begin();
                auto end   = grpvec3.end();
                CHECK((end - start) == 3);

                auto                    it = grpvec3.begin();
                matter::const_any_group grp{*it};
                CHECK(grp == ident.ordered_ids<int, float, short>());
                ++it;
                grp = matter::const_any_group{*it};
                CHECK(grp == ident.ordered_ids<int, char, short>());
                ++it;
                grp          = matter::const_any_group{*it};
                auto mut_grp = matter::any_group{*it};
                CHECK(grp == ident.ordered_ids<float, short, char>());
                ++it;
                CHECK(it == grpvec3.end());

                SECTION("exact groups")
                {
                    auto exact_grp =
                        matter::group(ident.ids<short, char, float>(), mut_grp);

                    auto comp_view =
                        exact_grp.emplace_back(std::forward_as_tuple(5),
                                               std::forward_as_tuple('n'),
                                               std::forward_as_tuple(5.0f));

                    auto [s, c, f] = comp_view;
                    CHECK(s == 5);
                    CHECK(c == 'n');
                    CHECK(f == 5.0f);
                }
            }
        }

        SECTION("reverse iterator")
        {
            SECTION("group_vector size 1")
            {
                auto start = grpvec1.rbegin();
                auto end   = grpvec1.rend();
                CHECK((end - start) == 4);
                CHECK(grpvec1.size() == 4);

                auto rit = grpvec1.rbegin();
                CHECK((*rit).contains(ident.id<char>()));
                ++rit;
                CHECK((*rit).contains(ident.id<short>()));
                ++rit;
                CHECK((*rit).contains(ident.id<float>()));
                ++rit;
                CHECK((*rit).contains(ident.id<int>()));
                ++rit;
                CHECK(rit == grpvec1.rend());
            }

            SECTION("group_vector size 2")
            {
                auto start = grpvec2.rbegin();
                auto end   = grpvec2.rend();
                CHECK((end - start) == 2);
                CHECK(grpvec2.size() == 2);

                auto                    it = grpvec2.rbegin();
                matter::const_any_group grp{*it};
                CHECK(grp == ident.ordered_ids<short, char>());
                ++it;
                grp = matter::const_any_group{*it};
                CHECK(grp == ident.ordered_ids<float, int>());
                ++it;
                CHECK(it == grpvec2.rend());
            }

            SECTION("group_vector size 3")
            {
                auto start = grpvec3.rbegin();
                auto end   = grpvec3.rend();
                CHECK((end - start) == 3);
                CHECK(grpvec3.size() == 3);

                auto                    it = grpvec3.rbegin();
                matter::const_any_group grp{*it};
                CHECK(grp == ident.ordered_ids<float, short, char>());
                ++it;
                grp = matter::const_any_group{*it};
                CHECK(grp == ident.ordered_ids<int, char, short>());
                ++it;
                grp = matter::const_any_group{*it};
                CHECK(grp == ident.ordered_ids<int, float, short>());
                ++it;
                CHECK(it == grpvec3.rend());
            }
        }

        SECTION("find group")
        {
            ident.register_type<uint8_t>();

            auto it = grpvec1.find(ident.ordered_ids<uint8_t>());
            CHECK(it == grpvec1.end());
            CHECK(!grpvec1.find_group(ident.ids<uint8_t>()).has_value());

            auto grp = grpvec1.find_emplace_group(ident.ids<uint8_t>());
            CHECK(grp == ident.ordered_ids<uint8_t>());

            it = grpvec1.find(ident.ordered_ids<float>());
            CHECK(it != grpvec1.end());
            CHECK(grpvec1.find_group(ident.ids<float>()).has_value());
        }

        SECTION("view")
        {
            SECTION("forward")
            {
                auto grp_view =
                    matter::group_vector_view{ident.ids<char>(), grpvec3};
                auto find_grp_it =
                    grpvec3.find(ident.ordered_ids<int, short, char>());
                auto it = grp_view.begin();

                // no known comparison implemented at this point in time
                // CHECK(*find_grp_it == *it);

                ++it;
                find_grp_it =
                    grpvec3.find(ident.ordered_ids<short, char, float>());
                CHECK(it != grp_view.end());

                // CHECK(*find_grp_it == *it);

                ++it;
                CHECK(it == grp_view.end());
            }

            SECTION("reverse")
            {
                auto grp_view =
                    matter::group_vector_view{ident.ids<int>(), grpvec3};
                auto view_it = grp_view.rbegin(); // int, short, char

                ++view_it; // float, int, short
                CHECK(view_it != grp_view.rend());

                ++view_it; // 1 past the end
                CHECK(view_it == grp_view.rend());
            }
        }

        SECTION("insert_back")
        {
            auto grp =
                grpvec3.find_group(ident.ids<float, int, short>()).value();
            std::vector<float> fvec;
            std::vector<int>   ivec;
            std::vector<short> svec;

            for (auto i = 0; i < 10; ++i)
            {
                fvec.push_back(i);
                ivec.push_back(i);
                svec.push_back(i);
            }

            grp.insert_back(std::pair{fvec.begin(), fvec.end()},
                            std::pair{ivec.begin(), ivec.end()},
                            std::pair{svec.begin(), svec.end()});

            auto view = matter::group_view{grp};

            auto i = 0;
            matter::for_each(view.begin(), view.end(), [&](auto comp_view) {
                auto [fcomp, icomp, scomp] = comp_view;
                CHECK(fcomp == i);
                CHECK(icomp == i);
                CHECK(scomp == i);
                ++i;
            });
        }
    }
}
