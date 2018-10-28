#include <catch2/catch.hpp>

#include <string>

#include "matter/entity/entity.hpp"
#include "matter/entity/entity_manager.hpp"
#include "matter/entity/entity_traits.hpp"

struct custom_entity
{
    using id_type      = std::string;
    using version_type = int;

private:
    id_type      m_id;
    version_type m_ver;

public:
    const id_type id() const
    {
        return m_id;
    }

    const version_type version() const
    {
        return m_ver;
    }

    bool operator==(const custom_entity& other) const
    {
        return m_ver == other.m_ver && m_id.compare(other.m_id) == 0;
    }

    explicit operator bool() const
    {
        return !std::empty(m_id);
    }
};

TEST_CASE("entity")
{
    SECTION("traits")
    {
        REQUIRE(matter::is_entity_v<matter::entity<uint32_t, uint32_t>>);
    }
}

TEST_CASE("entity manager")
{
    matter::entity_manager<matter::entity<uint32_t, uint32_t>> manager;

    CHECK(std::size(manager) == 0);

    SECTION("create")
    {
        auto ent1 = manager.create();
        auto ent2 = manager.create();

        CHECK(ent1 != ent2);
        CHECK(std::size(manager) == 2);
        CHECK(manager.is_valid(ent1));
        CHECK(manager.is_valid(ent2));

        SECTION("destroy");
        {
            manager.destroy(ent1);
            manager.destroy(ent2);

            CHECK(std::size(manager) == 0);
            CHECK(!manager.is_valid(ent1));
            CHECK(!manager.is_valid(ent2));

            SECTION("create version increment")
            {
		auto capacity = manager.capacity();
		auto ent3 = manager.create();
		auto ent4 = manager.create();

		CHECK(capacity == manager.capacity());

		CHECK(ent1 != ent3);
		CHECK(ent2 != ent4);

		CHECK(ent1.id() == ent3.id());
		CHECK(ent2.id() == ent4.id());
	    }
        }
    }
}
