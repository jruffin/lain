/*
 * Serialization Experiments Lain
 *
 * Copyright (C) 2016 Julien Ruffin and project contributors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "ThreadContainerStack.h"
#include <cassert>

namespace Lain {

class AbstractField;

class ContainerBase
{
public:
    ContainerBase()
    {
        _Lain_pushContainerStack();
    }

    // Note we do not copy anything from the other object
    // because the field list has to be rebuilt locally
    ContainerBase(const ContainerBase& rhs)
    {
        _Lain_pushContainerStack();
    }

    ContainerBase& operator=(const ContainerBase& rhs)
    {
        return *this;
    }

    // Note we do not move anything from the other object
    // because the field list has to be rebuilt locally
    ContainerBase(ContainerBase&& rhs)
    {
        _Lain_pushContainerStack();
    }

    // No need for this since the copy assignment does not
    // do anything either
    ContainerBase& operator=(ContainerBase&& rhs) = delete;

    void _Lain_initDone()
    {
        auto& stack = internal::getThreadContainerStack();

        assert(!stack.empty());
        assert(stack.top() == this);

        // Pop the current structure stack
        if (!stack.empty()) {
            stack.pop();
        }
    }

private:
    void _Lain_pushContainerStack()
    {
        // Initialize the TLS, push this onto
        // the thread's current structure stack
        internal::getThreadContainerStack().push(this);
    }

protected:
    friend class FieldBase;

    void _Lain_addField(AbstractField& f)
    {
        _Lain_fields.push_back(&f);
    }

    std::list<AbstractField*> _Lain_fields;
};

};

