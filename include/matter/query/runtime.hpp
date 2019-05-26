#ifndef MATTER_QUERY_RUNTIME_HPP
#define MATTER_QUERY_RUNTIME_HPP

#pragma once

namespace matter
{
enum struct access
{
    read,
    write,
    inaccessible
};

// the enum used to get information in a dynamic context
enum struct presence
{
    require,
    optional,
    exclude
};
} // namespace matter

#endif
