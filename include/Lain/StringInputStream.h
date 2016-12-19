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
#include "AbstractStream.h"

namespace Lain {

class StringInputStream : public AbstractInputStream
{
public:
    StringInputStream(const std::string& s) : ss(s) {}

    template <typename T>
    void get(const std::string& name, T& d)
    {
        ss >> d;
    }

private:
    std::stringstream ss;
};

template <>
void StringInputStream::get<StructureStart>(const std::string& name, StructureStart& d)
{
}

template <>
void StringInputStream::get<StructureEnd>(const std::string& name, StructureEnd& d)
{
}

};

