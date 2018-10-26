#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>

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
};

TEST_CASE("entity")
{
    SECTION("traits")
    {
        REQUIRE(matter::is_entity<custom_entity>::value);
    }
}
