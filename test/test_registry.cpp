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

    SECTION("group comparison")
    {

        reg.register_component<const char*>();
        reg.register_component<char>();

        // ids should roughly be 1, 0, 3 -> 0, 1, 3
        auto grp_vec = matter::group_vector{3};
        auto grp     = grp_vec.emplace(
            reg.component_ids<int_comp, float_comp, const char*>());

        SECTION("contains")
        {
            // single components
            CHECK(grp.contains(reg.component_id<float_comp>()));
            CHECK(grp.contains(reg.component_id<int_comp>()));
            CHECK(grp.contains(reg.component_id<const char*>()));
        }

        SECTION("get multiple")
        {
            // copy group in wrong order
            auto other_grp =
                matter::group<float_comp, const char*, int_comp>{grp};
            CHECK(other_grp.empty());
            CHECK(grp.empty());

            auto str = "hello_world";
            auto f   = 5.0f;
            auto i   = 500;

            // emplace some things
            other_grp.emplace_back(std::forward_as_tuple(f),
                                   std::forward_as_tuple(str),
                                   std::forward_as_tuple(i));

            CHECK(!other_grp.empty());
            CHECK(!grp.empty());

            auto&& [fcomp, cstr, icomp] = other_grp[0];

            CHECK(fcomp.f == f);
            CHECK(std::char_traits<char>::compare(
                      cstr, str, std::char_traits<char>::length(cstr)) == 0);
            CHECK(icomp.i == i);
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
        auto ids  = reg.component_ids<float_comp>();
        auto view = reg.view(ids);

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
