#pragma once

namespace TricksAndThings { namespace detail {

struct AlwaysYes
{
    bool operator()() { return true; }
};

} }
