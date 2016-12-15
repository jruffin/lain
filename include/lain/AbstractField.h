#pragma once
#include "AbstractStream.h"

namespace lain {

struct AbstractField
{
    virtual ~AbstractField() {}

    virtual const std::string& getName() const = 0;

    virtual void read(AbstractInputStream&) = 0;
    virtual void write(AbstractOutputStream&) = 0;
};

};

