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

#include <string>
#include <sstream>
#include <stdexcept>
#include "Field.h"

namespace Lain {

template <typename Context, typename T>
class Constant : public Field<Context, T>
{
public:
    T const & value;

    Constant()
        : handle(*this)
    { }

    Constant(const std::string& name)
        : Field<Context, T>(name), handle(*this)
    { }

    Constant(const std::string& name, const T& v)
        : Field<Context, T>(name), value(mutableValue), mutableValue(v), handle(*this) 
    { }

    ~Constant()
    { }

    virtual void read(typename Context::InputStreamType& s) override
    {
        T readValue;
        s.get(this->getName(), readValue);
        if (readValue != value) {
            std::stringstream ss;
            ss  << "Constant " << this->getName() << " read from stream "
                << "should have value " << value << ", but has value "
                << readValue;
            throw std::runtime_error(ss.str());
        }
    }

    virtual void write(typename Context::OutputStreamType& s) override
    {
        s.put(this->getName(), value);
    }

    virtual T const& getContents() const override
    {
        return value;
    }

    virtual void setContents(T const& c) override
    {
        mutableValue = c;
    }

private:
    T mutableValue;
    TypeErasedFieldHandle<Context, T> handle;
};


};
