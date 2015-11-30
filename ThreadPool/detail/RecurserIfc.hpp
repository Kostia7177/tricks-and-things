#pragma once

#include<memory>

namespace TricksAndThings { namespace detail
{

struct RecurserIfc
{
    virtual bool overridePush() = 0;
    virtual ~RecurserIfc() {}
};

typedef std::unique_ptr<detail::RecurserIfc> RecurserPtr;

} }
