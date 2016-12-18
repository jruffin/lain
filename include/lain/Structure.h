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

#include <list>
#include <stack>
#include <algorithm>
#include <string>
#include <memory>

#include "ContainerBase.h"
#include "AbstractStream.h"
#include "Field.h"

namespace lain {

struct StructureStart {};
struct StructureEnd {};

template <typename Context>
class StructureBase :
    public FieldBase,
    public ContainerBase
{
    public:
        StructureBase() {}
        StructureBase(const std::string& name) : FieldBase(name) {}

        void read(AbstractInputStream& s) override
        {
            StructureStart start;
            toConcreteType<Context>(s).get(getName(), start);

            std::for_each(_lain_fields.begin(), _lain_fields.end(),
                    [&s](auto& it) { it->read(s); });

            StructureEnd end;
            toConcreteType<Context>(s).get(getName(), end);
        }

        void write(AbstractOutputStream& s) override
        {
            toConcreteType<Context>(s).put(getName(), StructureStart());

            std::for_each(_lain_fields.begin(), _lain_fields.end(),
                    [&s](auto& it) { it->write(s); });

            toConcreteType<Context>(s).put(getName(), StructureEnd());
        }
};

template <typename Context, template <typename> typename StructureType>
struct Structure
: public StructureBase<Context>,
  public StructureType<Context>
{
    Structure()
    {
        StructureBase<Context>::_lain_initDone();
    }

    Structure(const std::string& name)
        : StructureBase<Context>(name), StructureType<Context>()
    {
        StructureBase<Context>::_lain_initDone();
    }
};



};
