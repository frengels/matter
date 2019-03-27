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

        SECTION("sorted")
        {
            SECTION("group_vector size 1")
            {
                // all the emplaced stores should be correctly sorted now
                auto it = grpvec1.begin();
                CHECK(it->id() == ident.id<int>());
                ++it;
                CHECK(it->id() == ident.id<float>());
                ++it;
                CHECK(it->id() == ident.id<short>());
                ++it;
                CHECK(it->id() == ident.id<char>());
                ++it;
                CHECK(it == grpvec1.end());
            }

            SECTION("group_vector size 2")
            {
                auto                    it = grpvec2.begin();
                matter::const_any_group grp{*it, 2};
                CHECK(grp == ident.ordered_ids<float, int>());
                ++it;
                grp = matter::const_any_group{*it, 2};
                CHECK(grp == ident.ordered_ids<short, char>());
                ++it;
                CHECK(it == grpvec2.end());
            }

            SECTION("group_vector size 3")
            {
                auto                    it = grpvec3.begin();
                matter::const_any_group grp{*it, 3};
                CHECK(grp == ident.ordered_ids<int, float, short>());
                ++it;
                grp = matter::const_any_group{*it, 3};
                CHECK(grp == ident.ordered_ids<int, char, short>());
                ++it;
                grp          = matter::const_any_group{*it, 3};
                auto mut_grp = matter::any_group{*it, 3};
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
                auto rit = grpvec1.rbegin();
                CHECK(rit->id() == ident.id<char>());
                ++rit;
                CHECK(rit->id() == ident.id<short>());
                ++rit;
                CHECK(rit->id() == ident.id<float>());
                ++rit;
                CHECK(rit->id() == ident.id<int>());
                ++rit;
                CHECK(rit == grpvec1.rend());
            }

            SECTION("group_vector size 2")
            {
                auto                    it = grpvec2.rbegin();
                matter::const_any_group grp{*it, 2};
                CHECK(grp == ident.ordered_ids<short, char>());
                ++it;
                grp = matter::const_any_group{*it, 2};
                CHECK(grp == ident.ordered_ids<float, int>());
                ++it;
                CHECK(it == grpvec2.rend());
            }

            SECTION("group_vector size 3")
            {
                auto                    it = grpvec3.rbegin();
                matter::const_any_group grp{*it, 3};
                CHECK(grp == ident.ordered_ids<float, short, char>());
                ++it;
                grp = matter::const_any_group{*it, 3};
                CHECK(grp == ident.ordered_ids<int, char, short>());
                ++it;
                grp = matter::const_any_group{*it, 3};
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

                CHECK(*find_grp_it == *it);

                ++it;
                find_grp_it =
                    grpvec3.find(ident.ordered_ids<short, char, float>());

                CHECK(*find_grp_it == *it);

                ++it;
                CHECK(it == grp_view.end());
            }

            SECTION("reverse")
            {
                auto grp_view =
                    matter::group_vector_view{ident.ids<int>(), grpvec3};
                auto find_grp_it =
                    grpvec3.find(ident.ordered_ids<int, short, char>());
                CHECK(find_grp_it != grpvec3.end());
                auto view_it = grp_view.rbegin();

                CHECK(*find_grp_it == *view_it);

                ++view_it;
                find_grp_it =
                    grpvec3.find(ident.ordered_ids<int, float, short>());

                CHECK(*find_grp_it == *view_it);

                ++view_it;
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
            std::for_each(view.begin(), view.end(), [&](auto comp_view) {
                auto [fcomp, icomp, scomp] = comp_view;
                CHECK(fcomp == i);
                CHECK(icomp == i);
                CHECK(scomp == i);
                ++i;
            });
        }
    }
}
