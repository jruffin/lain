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

namespace Lain {

struct AbstractStream
{
    virtual ~AbstractStream() {}

    template <typename T>
    T& as()
    {
        return dynamic_cast<T&>(*this);
    }
};

struct AbstractInputStream : public AbstractStream {};
struct AbstractOutputStream : public AbstractStream {};


struct NullOutputStream : public AbstractOutputStream
{
    template <typename T>
    void put(const std::string& name, const T& d) {}
};

struct NullInputStream : public AbstractInputStream
{
    template <typename T>
    void get(const std::string& name, T& d) {}
};


};

