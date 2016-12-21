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
    public Field< Context, std::list<FieldHandle*> >
{
public:
    StructureBase() : handle(*this) {}
    StructureBase(const std::string& name)
        : Field< Context, std::list<FieldHandle*> >(name),
          handle(*this)
    {
    }

    StructureBase(const StructureBase<Context>& rhs) = default;
    StructureBase<Context>& operator=(const StructureBase<Context>& rhs) = default;

    StructureBase(StructureBase<Context>&& rhs) = default;
    StructureBase<Context>& operator=(StructureBase<Context>&& rhs) = default;

    void read(typename Context::InputStreamType & s) override
    {
        StructureStart start;
        s.get(this->getName(), start);

        auto& fields = containerBase.getFields();

        std::for_each(fields.begin(), fields.end(),
                [&s](auto& it) { it->read(s); });

        StructureEnd end;
        s.get(this->getName(), end);
    }

    void write(typename Context::OutputStreamType& s) override
    {
        s.put(this->getName(), StructureStart());

        auto& fields = containerBase.getFields();

        std::for_each(fields.begin(), fields.end(),
                [&s](auto& it) { it->write(s); });

        s.put(this->getName(), StructureEnd());
    }

    std::list<FieldHandle*> const& getContents() const override
    {
        // The data that the other structure really
        // needs to copy us is actually the field list.
        return containerBase.getFields();
    }

    void setContents(std::list<FieldHandle*> const& c) override
    {
        std::list<FieldHandle*>::const_iterator sourceIt;
        std::list<FieldHandle*>::const_iterator destIt;

        auto& fields = containerBase.getFields();

        assert(c.size() == fields.size());

        for (sourceIt = c.cbegin(), destIt = fields.begin();
                sourceIt != c.cend() && destIt != fields.end();
                ++sourceIt, ++destIt) {
            // Copy the contents
            (*destIt)->copyContentsFrom(**sourceIt);
        }

    }

protected:
    // These two members MUST be declared in EXACTLY THAT ORDER,
    // otherwise the structure registers itself as a field of itself
    // with hilariously recursive results.
    TypeErasedFieldHandle<Context, std::list<FieldHandle*> > handle;
    ContainerBase containerBase;

};

template <typename Context, template <typename> typename StructureType>
struct Structure
: public StructureBase<Context>,
  public StructureType<Context>
{
public:
    Structure()
    {
        StructureBase<Context>::containerBase.initDone();
    }

    Structure(const std::string& name)
        : StructureBase<Context>(name), StructureType<Context>()
    {
        StructureBase<Context>::containerBase.initDone();
    }

    // This allows assigning from a structure of the same type
    // but with a different context. Context switching.
    template <typename OtherContext>
    Structure(const Structure<OtherContext, StructureType>& rhs)
    : StructureBase<Context>(rhs.getName()), StructureType<Context>()
    {
        StructureBase<Context>::containerBase.initDone();
        this->setContents(rhs.getContents());
    }

    // Assignment within the same context,
    // basically a special case.
    Structure(const Structure<Context, StructureType>& rhs)
        : StructureBase<Context>(rhs.getName()), StructureType<Context>()
    {
        StructureBase<Context>::containerBase.initDone();
        this->setContents(rhs.getContents());
    }

    // This allows assigning from a structure of the same type
    // but with a different context. Context switching.
    template <typename OtherContext>
    Structure<Context, StructureType>&
        operator=(const Structure<OtherContext, StructureType>& rhs)
    {
        this->setName(rhs.getName());
        this->setContents(rhs.getContents());
        return *this;
    }

    // Assignment within the same context,
    // basically a special case.
    Structure<Context, StructureType>&
        operator=(const Structure<Context, StructureType>& rhs)
    {
        this->setName(rhs.getName());
        this->setContents(rhs.getContents());
        return *this;
    }


    // No move semantics are possible here. Yet.
    Structure(Structure<Context, StructureType>&& rhs) = delete;
    Structure<Context, StructureType>&
        operator=(Structure<Context, StructureType>&& rhs) = delete;

};



};
