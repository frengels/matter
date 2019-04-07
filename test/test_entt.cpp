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

        std::vector<position> pos(1000000);
        std::vector<velocity> vel(1000000);

        reg.insert(std::pair{pos.begin(), pos.end()},
                   std::pair{vel.begin(), vel.end()});
    }

    SECTION("assign_comp_runtime")
    {
        matter::registry<> reg;
        reg.register_component<position>();
        reg.register_component<velocity>();

        timer t{"Constructing 1000000 runtime component pairs of position and "
                "velocity"};

        std::vector<position> pos(1000000);
        std::vector<velocity> vel(1000000);

        reg.insert(std::pair{pos.begin(), pos.end()},
                   std::pair{vel.begin(), vel.end()});
    }

    SECTION("iterate_single")
    {
        matter::registry<position, velocity> reg;
        std::vector<position>                pos(1000000);

        reg.insert(std::pair{pos.begin(), pos.end()});

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

        SECTION("iterator const")
        {
            timer t{
                "Iterating over 1000000 single components - const iterator"};

            auto pos_view = reg.view<position>();

            matter::for_each(
                pos_view.begin(), pos_view.end(), [](const auto&) {});
        }

        SECTION("iterator mut")
        {
            timer t{"Iterating over 1000000 single components - mut iterator"};

            auto pos_view = reg.view<position>();

            matter::for_each(
                pos_view.begin(), pos_view.end(), [](auto&& comp_view) {
                    comp_view.invoke(
                        [](position& position) { position.x = {}; });
                });
        }
    }
    // TODO: test destroying

    SECTION("iterate_double")
    {
        matter::registry<position, velocity> reg;
        std::vector<position>                pos(1000000);
        std::vector<velocity>                vel(1000000);

        reg.insert(std::pair{pos.begin(), pos.end()},
                   std::pair{vel.begin(), vel.end()});

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
        std::vector<velocity>                vel(1000000);

        reg.insert(std::pair{vel.begin(), vel.end()});

        std::vector<position> pos(1000000);

        reg.insert(std::pair{pos.begin(), pos.end()},
                   std::pair{vel.begin(), vel.end()});

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
        std::vector<velocity>                vel(1000000);

        reg.insert(std::pair{vel.begin(), vel.end()});

        reg.create<position, velocity>(std::forward_as_tuple(),
                                       std::forward_as_tuple());

        SECTION("const")
        {
            timer t{
                "Iterating over 1000000 components, only one has both - const"};

            auto view = reg.view<position, velocity>();
            view.for_each([](const position&, const velocity&) {});
        }

        SECTION("mutable")
        {
            timer t{
                "Iterating over 1000000 components, only one has both - mut"};

            auto view = reg.view<position, velocity>();
            view.for_each([](position& pos, velocity& vel) {
                pos.x = {};
                vel.x = {};
            });
        }
    }
}
