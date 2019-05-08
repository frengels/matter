#include <benchmark/benchmark.h>

#include "matter/component/registry.hpp"

void insert_single_vector(benchmark::State& state)
{
    std::vector<int> vec;

    while (state.KeepRunning())
    {
        vec.emplace_back(42);
    }
}

BENCHMARK(insert_single_vector)->Range(1, 1);

void insert_single_first_idx(benchmark::State& state)
{
    matter::
        registry<matter::unsigned_id<std::size_t>, int, char, float, uint64_t>
            reg;

    while (state.KeepRunning())
    {
        reg.create<int>(std::forward_as_tuple(42));
    }
}

BENCHMARK(insert_single_first_idx)->Range(1, 1);

void insert_single_dynamic(benchmark::State& state)
{
    matter::registry<matter::unsigned_id<std::size_t>,
                     std::array<int, 16>,
                     float,
                     char,
                     uint16_t>
        reg;
    reg.register_component<int>();

    while (state.KeepRunning())
    {
        reg.create<int>(std::forward_as_tuple(42));
    }
}

BENCHMARK(insert_single_dynamic)->Range(1, 1);

void insert_triple_static(benchmark::State& state)
{
    matter::registry<matter::unsigned_id<std::size_t>, int, float, char> reg;

    while (state.KeepRunning())
    {
        reg.create<int, float, char>(std::forward_as_tuple(42),
                                     std::forward_as_tuple(69.0f),
                                     std::forward_as_tuple('!'));
    }
}

BENCHMARK(insert_triple_static)->Range(1, 1);

void insert_triple_static_out_of_order(benchmark::State& state)
{
    matter::registry<matter::unsigned_id<std::size_t>, int, float, char> reg;

    while (state.KeepRunning())
    {
        reg.create<char, int, float>(std::forward_as_tuple('!'),
                                     std::forward_as_tuple(42),
                                     std::forward_as_tuple(69.0f));
    }
}

BENCHMARK(insert_triple_static_out_of_order)->Range(1, 1);

void insert_triple_mixed(benchmark::State& state)
{
    matter::registry<matter::unsigned_id<std::size_t>, int, float, char> reg;
    reg.register_component<uint16_t>();
    reg.register_component<uint8_t>();

    while (state.KeepRunning())
    {
        reg.create<int, uint16_t, uint8_t>(std::forward_as_tuple(42),
                                           std::forward_as_tuple(0),
                                           std::forward_as_tuple(255));
    }
}

BENCHMARK(insert_triple_mixed)->Range(1, 1);

void insert_triple_runtime(benchmark::State& state)
{
    matter::registry<matter::unsigned_id<std::size_t>, float, char> reg;
    reg.register_component<int>();
    reg.register_component<uint16_t>();
    reg.register_component<uint8_t>();

    while (state.KeepRunning())
    {
        reg.create<int, uint16_t, uint8_t>(std::forward_as_tuple(42),
                                           std::forward_as_tuple(0),
                                           std::forward_as_tuple(255));
    }
}

BENCHMARK(insert_triple_runtime)->Range(1, 1);

int main(int argc, char** argv)
{
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
}
