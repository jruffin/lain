#pragma once
#include "ThreadContainerStack.h"

namespace lain {

class AbstractField;

class ContainerBase
{
public:
    ContainerBase()
    {
        // Initialize the TLS, push this onto
        // the thread's current structure stack
        internal::getThreadContainerStack().push(this);
    }

    void _lain_initDone()
    {
        auto& stack = internal::getThreadContainerStack();
        // Pop the current structure stack
        if (!stack.empty()) {
            stack.pop();
        }
    }

protected:
    friend class FieldBase;

    void _lain_addField(AbstractField& f)
    {
        _lain_fields.push_back(&f);
    }

    // TODO implement the operator= and such
    // to *not* copy the contents of _lain_fields
    // but reinitialize the list instead


    std::list<AbstractField*> _lain_fields;
};

};

