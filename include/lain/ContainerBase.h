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

