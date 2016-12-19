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

namespace Lain {

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

        StructureBase(const StructureBase<Context>& rhs) = default;
        StructureBase<Context>& operator=(const StructureBase<Context>& rhs) = default;

        StructureBase(StructureBase<Context>&& rhs) = default;
        StructureBase<Context>& operator=(StructureBase<Context>&& rhs) = default;

        void read(AbstractInputStream& s) override
        {
            StructureStart start;
            toConcreteType<Context>(s).get(getName(), start);

            std::for_each(_Lain_fields.begin(), _Lain_fields.end(),
                    [&s](auto& it) { it->read(s); });

            StructureEnd end;
            toConcreteType<Context>(s).get(getName(), end);
        }

        void write(AbstractOutputStream& s) override
        {
            toConcreteType<Context>(s).put(getName(), StructureStart());

            std::for_each(_Lain_fields.begin(), _Lain_fields.end(),
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
        StructureBase<Context>::_Lain_initDone();
    }

    Structure(const std::string& name)
        : StructureBase<Context>(name), StructureType<Context>()
    {
        StructureBase<Context>::_Lain_initDone();
    }

    Structure(const Structure<Context, StructureType>& rhs)
        : StructureBase<Context>(rhs), StructureType<Context>(rhs)
    {
        StructureBase<Context>::_Lain_initDone();
    }

    Structure<Context, StructureType>&
        operator=(const Structure<Context, StructureType>& rhs)
    {
        static_cast<StructureBase<Context>&>(*this) = rhs;
        static_cast<StructureType<Context>&>(*this) = rhs;

        return *this;
    }


    Structure(Structure<Context, StructureType>&& rhs)
        : StructureBase<Context>(std::move(rhs)), StructureType<Context>(std::move(rhs))
    {
        StructureBase<Context>::_Lain_initDone();
    }

    Structure<Context, StructureType>&
        operator=(Structure<Context, StructureType>&& rhs)
    {
        static_cast<StructureBase<Context>&>(*this) = std::move(rhs);
        static_cast<StructureType<Context>&>(*this) = std::move(rhs);

        return *this;
    }

};



};
