#include <catch2/catch.hpp>

#include <string_view>

#include "matter/component/group_vector.hpp"
#include "matter/component/registry.hpp"
#include "matter/component/registry_view.hpp"
#include "matter/component/traits.hpp"
#include "matter/util/erased.hpp"

struct float_comp
{
    float f;

    constexpr float_comp(float f) : f{f}
    {}
};

struct int_comp
{
    int i;

    constexpr int_comp(int i) : i{i}
    {}
};

struct string_comp
{
    std::string_view str;
};

TEST_CASE("registry")
{
    matter::registry<float_comp, int_comp, string_comp> reg;

    SECTION("group")
    {
        auto grp_vec = matter::group_vector{1};

        auto grp = grp_vec.emplace(
            matter::unordered_typed_ids{reg.component_id<int_comp>()});

        CHECK(grp.contains(reg.component_id<int_comp>()));
        CHECK(!grp.contains(reg.component_id<string_comp>()));
        CHECK(!grp.contains(reg.component_id<float_comp>()));

        auto& vector = grp.storage(reg.component_id<int_comp>());

        // just test whether the retrieve vector is correct
        vector.emplace_back(5);
        CHECK(vector.size() == 1);
        CHECK(vector[0].i == 5);
    }

    SECTION("group comparison")
    {

        reg.register_component<const char*>();
        reg.register_component<char>();

        // ids should roughly be 1, 0, 3 -> 0, 1, 3
        auto grp_vec = matter::group_vector{3};
        auto grp     = grp_vec.emplace(
            reg.component_ids<int_comp, float_comp, const char*>());

        SECTION("<")
        {

            // 0, 1, 3 < 2, 1, 4
            CHECK(grp < matter::ordered_typed_ids{
                            reg.component_ids<float_comp, int_comp, char>()});
            // should be 0, 1, 3 < 0, 1, 4
            CHECK(grp < matter::ordered_typed_ids{
                            reg.component_ids<float_comp, int_comp, char>()});
        }

        SECTION("contains")
        {
            // single components
            CHECK(grp.contains(reg.component_id<float_comp>()));
            CHECK(grp.contains(reg.component_id<int_comp>()));
            CHECK(grp.contains(reg.component_id<const char*>()));
        }

        SECTION("get multiple")
        {
            auto vecs = grp.storage(
                reg.component_ids<const char*, float_comp, int_comp>());

            CHECK(std::get<0>(vecs).empty());
            CHECK(std::get<1>(vecs).empty());
            CHECK(std::get<2>(vecs).empty());

            auto str = "hello_world";
            auto f   = 5.0f;
            auto i   = 500;

            // emplace some things
            std::get<0>(vecs).emplace_back(str);
            std::get<1>(vecs).emplace_back(f);
            std::get<2>(vecs).emplace_back(i);

            // now retrieve the arrays in a different order, still not in
            // correct id order
            auto vecs_retrieved = grp.storage(
                reg.component_ids<float_comp, const char*, int_comp>());

            CHECK(!std::get<0>(vecs_retrieved).empty());
            CHECK(!std::get<1>(vecs_retrieved).empty());
            CHECK(!std::get<2>(vecs_retrieved).empty());

            CHECK(std::get<0>(vecs_retrieved)[0].f == f);
            CHECK(std::char_traits<char>::compare(
                      std::get<1>(vecs_retrieved)[0],
                      str,
                      std::char_traits<char>::length(str)) == 0);
            CHECK(std::get<2>(vecs_retrieved)[0].i == i);
        }
    }

    SECTION("register")
    {
        reg.register_component<const char*>();
        reg.create<const char*>(std::forward_as_tuple("Hello world"));
    }

    SECTION("create")
    {
        reg.create<float_comp, int_comp>(std::forward_as_tuple(5.0f),
                                         std::forward_as_tuple(5));
    }

    SECTION("view")
    {
        auto view = reg.view<float_comp>();

        // empty view
        float f{};
        view.for_each([&](const auto& fcomp) { f += fcomp.f; });

        CHECK(f == 0.0f);

        SECTION("fill view")
        {
            float                   f1{};
            std::vector<float_comp> fvec;
            fvec.reserve(10000);

            for (auto i = 0; i < 10000; ++i)
            {
                f1 = i;
                fvec.emplace_back(f1);
            }

            reg.insert(std::pair{fvec.begin(), fvec.end()});

            SECTION("read view")
            {
                auto fview = reg.view<float_comp>();

                auto j = 0;

                fview.for_each([&](const auto& fcomp) {
                    CHECK(fcomp.f == j);
                    ++j;
                });
            }

            std::vector<float_comp> fvec2;
            fvec2.reserve(10000);
            std::vector<int_comp> ivec;
            ivec.reserve(10000);

            for (auto i = 0; i < 10000; ++i)
            {
                fvec2.emplace_back(1.0f);
                ivec.emplace_back(1);
            }

            reg.insert(std::pair{fvec2.begin(), fvec2.end()},
                       std::pair{ivec.begin(), ivec.end()});

            SECTION("check multiple component views")
            {
                auto fview = reg.view<float_comp>();

                auto j = 0;
                fview.for_each([&](const float_comp&) { ++j; });

                // both the single inserted and double inserted should be found
                CHECK(j == 20000);

                auto ifview = reg.view<int_comp, float_comp>();

                j = 0;
                ifview.for_each(
                    [&](const int_comp&, const float_comp&) { ++j; });
                CHECK(j == 10000);
            }
        }
    }
}
