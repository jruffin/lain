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
class Constant : public FieldBase
{
    public:
        Constant() {}
        Constant(const std::string& name) : FieldBase(name) {}
        Constant(const std::string& name, const T& value)
            : FieldBase(name), value(value) {}

        virtual void read(AbstractInputStream& s) override
        {
            T readValue;
            toConcreteType<Context>(s).get(getName(), readValue);
            if (readValue != value) {
                std::stringstream ss;
                ss  << "Constant " << getName() << " read from stream "
                    << "should have value " << value << ", but has value "
                    << readValue;
                throw std::runtime_error(ss.str());
            }
        }

        virtual void write(AbstractOutputStream& s) override
        {
            toConcreteType<Context>(s).put(getName(), value);
        }

        const T value;
};


};
