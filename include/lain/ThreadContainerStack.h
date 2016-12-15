#pragma once

namespace lain {

class ContainerBase;

namespace internal {

auto& getThreadContainerStack()
{
    static thread_local std::stack<ContainerBase*> _stack;
    return _stack;
}

};
};
