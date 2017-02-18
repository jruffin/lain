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
#include "AbstractField.h"

namespace Lain {

// Base template interface for all fields with all three aspects
template <typename Context, typename ContentType>
struct Field :
    public Named,
    public Data<ContentType>,
    public Serializable<Context>
{
    Field() {}
    Field(const std::string& name) : Named(name) {}
};

template <typename Context, typename ContentType>
class TypeErasedFieldHandle :
    public FieldHandle
{
public:
    TypeErasedFieldHandle(Field<Context, ContentType>& f)
        : field(&f)
    {
        doRegistration();
    }

    ~TypeErasedFieldHandle()
    {
        // TODO deregister if registered?
    }

    const std::string& getName() const override
    {
        return field->getName();
    }

    void read(AbstractInputStream& s) override
    {
        field->read(dynamic_cast<typename Context::InputStreamType&>(s));
    }

    void write(AbstractOutputStream& s) override
    {
        field->write(dynamic_cast<typename Context::OutputStreamType&>(s));
    }

    void setField(Field<Context, ContentType>* f)
    {
        field = f;
    }

    const TypeErasedData* getTypeErasedData() const override
    {
        return dynamic_cast<TypeErasedData*>(field);
    }

    void copyContentsFrom(const FieldHandle& h)
    {
        const TypeErasedData* otherField = h.getTypeErasedData();
        if (otherField) {
            // XXX This is a total leap of faith!
            // Make sure that the types are compatible when
            // calling this method!
            const Data<ContentType>* other = dynamic_cast<const Data<ContentType>*>(otherField);
            field->setContents(other->getContents());
        } else {
            throw std::runtime_error("Handle field pointer empty!");
        }
    }


private:
    // Underlying field
    Field<Context, ContentType>* field;

    void doRegistration()
    {
        auto stack = internal::getThreadContainerStack();
        if (!stack.empty()) {
            stack.top()->addField(*this);
        }
    }
};


template <typename Context, typename T>
class SimpleField : public Field<Context, T>
{
public:
    T value;

    SimpleField()
        : handle(*this)
    { }

    SimpleField(const std::string& name)
        : Field<Context, T>(name), handle(*this)
    { }

    SimpleField(const std::string& name, const T& value)
        : Field<Context, T>(name), value(value), handle(*this)
    { }

    ~SimpleField()
    { }

    virtual void read(typename Context::InputStreamType& s) override
    {
        s.get(this->getName(), value);
    }

    virtual void write(typename Context::OutputStreamType& s) override
    {
        s.put(this->getName(), value);
    }

    virtual T const& getContents() const override
    {
        return value;
    }

    virtual void setContents(T const & contents) override
    {
        value = contents;
    }

private:
    TypeErasedFieldHandle<Context, T> handle;

};

};

