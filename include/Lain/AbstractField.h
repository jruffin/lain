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

#include "AbstractStream.h"

namespace Lain {

// Base class for the named
// aspect of Lain fields.
class Named
{
public:
    const std::string& getName() const
    {
        return _name;
    }

    void setName(const std::string& n)
    {
        _name = n;
    }

protected:
    Named() : _name("(unnamed)") {}
    Named(const std::string& n) : _name(n) {}

private:
    std::string _name;
};

struct TypeErasedData
{
    virtual ~TypeErasedData() {};
};

// Base interface for the "field containing data of type ContentType"
// aspect of Lain fields.
template <typename ContentType>
struct Data : public TypeErasedData
{
    virtual ContentType const& getContents() const = 0;
    virtual void setContents(ContentType const & contents) = 0;
};

// Base interface for the "serializable data using streams
// of a type given by Context" aspect of Lain fields.
template <typename Context>
struct Serializable
{
    virtual void read(typename Context::InputStreamType& s) = 0;
    virtual void write(typename Context::OutputStreamType& s) = 0;
};

// Type-erased generic field handle.
struct FieldHandle
{
    virtual ~FieldHandle() {}

    virtual const std::string& getName() const = 0;

    virtual void read(AbstractInputStream&) = 0;
    virtual void write(AbstractOutputStream&) = 0;

    // Copy the contents from another field handle.
    // Be extremely careful with this method! It will only
    // work if both fields referred to implement Data<T> with
    // the same type.
    virtual void copyContentsFrom(const FieldHandle& other) = 0;

    // Returns a (type-erased) pointer to the underlying data field.
    virtual const TypeErasedData* getTypeErasedData() const = 0;
};

};

