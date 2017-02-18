/*
 * Serialization Experiments Lain
 *
 * Copyright (C) 2016-2017 Julien Ruffin and project contributors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "ThreadContainerStack.h"
#include <cassert>

namespace Lain {

class FieldHandle;

class ContainerBase
{
public:
    ContainerBase()
    {
        pushContainerStack();
    }

    // Note we do not copy anything from the other object
    // because the field list has to be rebuilt locally
    ContainerBase(const ContainerBase& rhs)
    {
        pushContainerStack();
    }

    ContainerBase& operator=(const ContainerBase& rhs)
    {
        return *this;
    }

    // Note we do not move anything from the other object
    // because the field list has to be rebuilt locally
    ContainerBase(ContainerBase&& rhs)
    {
        pushContainerStack();
    }

    // No need for this since the copy assignment does not
    // do anything either
    ContainerBase& operator=(ContainerBase&& rhs) = delete;

    void initDone()
    {
        auto& stack = internal::getThreadContainerStack();

        assert(!stack.empty());
        assert(stack.top() == this);

        // Pop the current structure stack
        if (!stack.empty()) {
            stack.pop();
        }
    }

    std::list<FieldHandle*> const& getFields() const
    {
        return fields;
    }

private:
    void pushContainerStack()
    {
        // Initialize the TLS, push this onto
        // the thread's current structure stack
        internal::getThreadContainerStack().push(this);
    }

    std::list<FieldHandle*> fields;


protected:
    template <typename Context, typename ContentType>
    friend class TypeErasedFieldHandle;

    void addField(FieldHandle& f)
    {
        fields.push_back(&f);
    }

};

};

