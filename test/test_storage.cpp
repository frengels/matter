#include <catch2/catch.hpp>

#include "matter/storage/sparse_vector_storage.hpp"

struct my_component
{
    int i;

    constexpr my_component(int i) : i{i}
    {}
};

TEST_CASE("sparse_vector_storage")
{
    matter::sparse_vector_storage<uint32_t, my_component> my_storage;

    SECTION("emplace")
    {
	auto index = GENERATE(as<uint32_t>(), 0, 1, 3, 5, 9, 100, 80);

	SECTION("values")
	{
		auto val = GENERATE(5, 4, 3, 9, 10, 300, 9001);

		REQUIRE(!my_storage.contains(index));
		my_storage.emplace(index, val);
		REQUIRE(my_storage.contains(index));
		REQUIRE(my_storage[index].i == val);
	}
    }

    SECTION("erase")
    {}

    SECTION("iterate")
    {}
}
