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
#include "AbstractField.h"

namespace lain {

class FieldBase : public AbstractField
{
public:
    FieldBase()
    {
        doRegistration();
    }

    FieldBase(const std::string& name)
        : _lain_name(name)
    {
        doRegistration();
    }

    void doRegistration()
    {
        auto stack = internal::getThreadContainerStack();
        if (!stack.empty()) {
            stack.top()->_lain_addField(*this);
        }
    }

    const std::string& getName() const override
    {
        return _lain_name;
    }

    virtual ~FieldBase() {}

    template <typename Context>
    typename Context::InputStreamType& toConcreteType(AbstractInputStream& s, Context* =0)
    {
        return s.as<typename Context::InputStreamType>();
    }

    template <typename Context>
    typename Context::OutputStreamType& toConcreteType(AbstractOutputStream& s, Context* =0)
    {
        return s.as<typename Context::OutputStreamType>();
    }

private:
    std::string _lain_name;
};


template <typename Context, typename T>
class Field : public FieldBase
{
public:
    T value;

    Field() {}
    Field(const std::string& name) : FieldBase(name) {}
    Field(const std::string& name, const T& value) : FieldBase(name), value(value) {}

    virtual void read(AbstractInputStream& s) override
    {
        toConcreteType<Context>(s).get(getName(), value);
    }

    virtual void write(AbstractOutputStream& s) override
    {
        toConcreteType<Context>(s).put(getName(), value);
    }
};

};

