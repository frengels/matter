#include <catch2/catch.hpp>

#include <chrono>
#include <iostream>
#include <string_view>

#include "matter/access/read.hpp"
#include "matter/access/readwrite.hpp"
#include "matter/access/write.hpp"
#include "matter/component/registry.hpp"

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
        matter::registry<matter::unsigned_id<std::size_t>, position, velocity>
            reg;

        timer t{"Constructing 1000000 static component pairs of position and "
                "velocity"};

        auto posvel = reg.create_buffer_for<position, velocity>();
        posvel.resize(1000000);

        reg.insert(posvel);
    }

    SECTION("assign_comp_runtime")
    {
        matter::registry<matter::unsigned_id<std::size_t>> reg;
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
        matter::registry<matter::unsigned_id<std::size_t>, position, velocity>
             reg;
        auto pos = reg.create_buffer_for<position>();
        pos.resize(1000000);

        reg.insert(pos);

        SECTION("access read")
        {
            timer t{"Iterating over 1000000 single components - read access"};

            auto pos_view = reg.view<matter::read<position>>();

            pos_view.for_each([](auto) {});
        }

        SECTION("access write")
        {
            timer t{
                "Iterating over 1000000 single components - readwrite access"};

            auto pos_view = reg.view<matter::readwrite<position>>();

            pos_view.for_each([](auto rwpos) {
                position pos = rwpos.get();
                pos.x        = {};
                rwpos.stage(pos);
            });
        }
    }

    SECTION("destroy")
    {
        matter::registry<matter::unsigned_id<std::size_t>, position> reg;

        auto buff = reg.create_buffer_for<position>();
        buff.resize(1000000);

        reg.insert(buff);

        timer t{"Destroying 1000000 entities"};

        /*
        auto pos_view = reg.view<position>();
        auto it       = pos_view.begin();
        for (std::size_t i = 999999; i != 0; --i)
        {
            pos_view.erase(it, i);
        }

        pos_view.erase(it, 0);
        */
    }

    SECTION("iterate_double")
    {
        matter::registry<matter::unsigned_id<std::size_t>, position, velocity>
             reg;
        auto posvel = reg.create_buffer_for<position, velocity>();
        posvel.resize(1000000);

        reg.insert(posvel);

        SECTION("read access")
        {
            timer t{"Iterating over 1000000 double components - read access"};

            auto view =
                reg.view<matter::read<position>, matter::read<velocity>>();

            view.for_each([](auto, auto) {});
        }

        SECTION("write access")
        {
            timer t{
                "Iterating over 1000000 double components - readwrite access"};

            auto view = reg.view<matter::readwrite<position>,
                                 matter::readwrite<velocity>>();

            view.for_each([](auto rwpos, auto rwvel) {
                auto pos = rwpos.get();
                auto vel = rwvel.get();

                pos.x = {};
                vel.x = {};

                rwpos.stage(pos);
                rwvel.stage(vel);
            });
        }
    }

    SECTION("iterate_double,half")
    {
        matter::registry<matter::unsigned_id<std::size_t>, position, velocity>
             reg;
        auto vel = reg.create_buffer_for<velocity>();
        vel.resize(1000000);

        reg.insert(vel);

        auto posvel = reg.create_buffer_for<position, velocity>(std::move(vel));
        posvel.resize(1000000);

        reg.insert(posvel);

        SECTION("read access")
        {
            timer t{"Iterating over 1000000 double components, only half "
                    "double - read"};

            auto view =
                reg.view<matter::read<position>, matter::read<velocity>>();

            view.for_each([](auto, auto) {});
        }

        SECTION("readwrite access")
        {
            timer t{"Iterating over 1000000 double components, only half "
                    "double - readwrite access"};

            auto view = reg.view<matter::readwrite<position>,
                                 matter::readwrite<velocity>>();

            view.for_each([](auto rwpos, auto rwvel) {
                auto pos = rwpos.get();
                auto vel = rwvel.get();

                pos.x = {};
                vel.x = {};

                rwpos.stage(pos);
                rwvel.stage(vel);
            });
        }
    }

    SECTION("iterate_single")
    {
        matter::registry<matter::unsigned_id<std::size_t>, position, velocity>
             reg;
        auto vel = reg.create_buffer_for<velocity>();
        vel.resize(1000000);

        reg.insert(vel);

        reg.create<position, velocity>(std::forward_as_tuple(),
                                       std::forward_as_tuple());

        SECTION("read access")
        {
            timer t{"Iterating over 1000000 components, only one has both - "
                    "read access"};

            auto view =
                reg.view<matter::read<position>, matter::read<velocity>>();
            view.for_each([](auto, auto) {});
        }

        SECTION("readwrite access")
        {
            timer t{"Iterating over 1000000 components, only one has both - "
                    "readwrite access"};

            auto view = reg.view<matter::readwrite<position>,
                                 matter::readwrite<velocity>>();
            view.for_each([](auto rwpos, auto rwvel) {
                auto pos = rwpos.get();
                auto vel = rwvel.get();

                pos.x = {};
                vel.x = {};

                rwpos.stage(pos);
                rwvel.stage(vel);
            });
        }
    }
}
