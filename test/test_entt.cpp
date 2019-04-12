#include <catch2/catch.hpp>

#include <chrono>
#include <iostream>
#include <string_view>

#include "matter/component/registry.hpp"
#include "matter/component/registry_view.hpp"

struct position
{
    std::uint64_t x;
    std::uint64_t y;
};

struct velocity
{
    std::uint64_t x;
    std::uint64_t y;
};

struct timer final
{
private:
    std::string_view                                   timer_name_;
    std::chrono::time_point<std::chrono::system_clock> start_;

public:
    timer(const std::string_view name)
        : timer_name_{name}, start_{[&]() {
              std::cout << timer_name_ << '\n';
              return std::chrono::system_clock::now();
          }()}
    {}

    ~timer()
    {
        auto now = std::chrono::system_clock::now();
        std::cout << std::chrono::duration<double>(now - start_).count()
                  << " seconds" << std::endl;
    }
};

// TODO: support entity_components to replicate the first 2 tests

TEST_CASE("benchmarks")
{
    SECTION("assign_comp_static")
    {
        matter::registry<position, velocity> reg;

        timer t{"Constructing 1000000 static component pairs of position and "
                "velocity"};

        auto posvel = reg.create_buffer_for<position, velocity>();
        posvel.resize(1000000);

        reg.insert(posvel);
    }

    SECTION("assign_comp_runtime")
    {
        matter::registry<> reg;
        reg.register_component<position>();
        reg.register_component<velocity>();

        timer t{"Constructing 1000000 runtime component pairs of position and "
                "velocity"};

        auto posvel = reg.create_buffer_for<velocity, position>();
        posvel.resize(1000000);

        reg.insert(posvel);
    }

    SECTION("iterate_single")
    {
        matter::registry<position, velocity> reg;
        auto pos = reg.create_buffer_for<position>();
        pos.resize(1000000);

        reg.insert(pos);

        SECTION("const")
        {
            timer t{"Iterating over 1000000 single components - const"};

            auto pos_view = reg.view<position>();

            pos_view.for_each([](const auto&) {});
        }

        SECTION("mutable")
        {
            timer t{"Iterating over 1000000 single components - mut"};

            auto pos_view = reg.view<position>();

            pos_view.for_each([](position& pos) { pos.x = {}; });
        }

        SECTION("group_view_iterator const")
        {
            timer t{"Iterating over 1000000 single components - const "
                    "group_view_iterator"};

            auto pos_view = reg.view<position>();

            matter::for_each(
                pos_view.begin(), pos_view.end(), [](auto grp_view) {
                    auto sz = grp_view.size();
#pragma omp simd
                    for (std::size_t i = 0; i < sz; ++i)
                    {
                        grp_view[i].invoke([](const position&) {});
                    }
                });
        }

        SECTION("group_view_iterator const")
        {
            timer t{"Iterating over 1000000 single components - const "
                    "group_view_iterator"};

            auto pos_view = reg.view<position>();

            matter::for_each(
                pos_view.begin(), pos_view.end(), [](auto grp_view) {
                    auto sz = grp_view.size();
#pragma omp simd
                    for (std::size_t i = 0; i < sz; ++i)
                    {
                        grp_view[i].invoke([](position& pos) { pos.x = {}; });
                    }
                });
        }
    }

    SECTION("destroy")
    {
        matter::registry<position> reg;

        auto buff = reg.create_buffer_for<position>();
        buff.resize(1000000);

        reg.insert(buff);

        timer t{"Destroying 1000000 entities"};

        auto pos_view = reg.view<position>();
        auto it       = pos_view.begin();
        for (std::size_t i = 999999; i != 0; --i)
        {
            pos_view.erase(it, i);
        }

        pos_view.erase(it, 0);
    }

    SECTION("iterate_double")
    {
        matter::registry<position, velocity> reg;
        auto posvel = reg.create_buffer_for<position, velocity>();
        posvel.resize(1000000);

        reg.insert(posvel);

        SECTION("const")
        {
            timer t{"Iterating over 1000000 double components - const"};

            auto view = reg.view<position, velocity>();

            view.for_each([](const position&, const velocity&) {});
        }

        SECTION("mutable")
        {
            timer t{"Iterating over 1000000 double components - mut"};

            auto view = reg.view<position, velocity>();

            view.for_each([](position& pos, velocity& vel) {
                pos.x = {};
                vel.x = {};
            });
        }
    }

    SECTION("iterate_double,half")
    {
        matter::registry<position, velocity> reg;
        auto vel = reg.create_buffer_for<velocity>();
        vel.resize(1000000);

        reg.insert(vel);

        auto posvel = reg.create_buffer_for<position, velocity>(std::move(vel));
        posvel.resize(1000000);

        reg.insert(posvel);

        SECTION("const")
        {
            timer t{"Iterating over 1000000 double components, only half "
                    "double - const"};

            auto view = reg.view<position, velocity>();

            view.for_each([](const position&, const velocity&) {});
        }

        SECTION("mutable")
        {
            timer t{"Iterating over 1000000 double components, only half "
                    "double - mut"};

            auto view = reg.view<position, velocity>();

            view.for_each([](position& pos, velocity& vel) {
                pos.x = {};
                vel.x = {};
            });
        }
    }

    SECTION("iterate_single")
    {
        matter::registry<position, velocity> reg;
        auto vel = reg.create_buffer_for<velocity>();
        vel.resize(1000000);

        reg.insert(vel);

        reg.create<position, velocity>(std::forward_as_tuple(),
                                       std::forward_as_tuple());

        SECTION("const")
        {
            timer t{"Iterating over 1000000 components, only one has both "
                    "- const"};

            auto view = reg.view<position, velocity>();
            view.for_each([](const position&, const velocity&) {});
        }

        SECTION("mutable")
        {
            timer t{"Iterating over 1000000 components, only one has both "
                    "- mut"};

            auto view = reg.view<position, velocity>();
            view.for_each([](position& pos, velocity& vel) {
                pos.x = {};
                vel.x = {};
            });
        }
    }
}
