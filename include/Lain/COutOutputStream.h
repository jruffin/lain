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

#include <string>
#include <iostream>
#include "AbstractStream.h"

namespace Lain {

struct COutOutputStream : public AbstractOutputStream
{
    template <typename T>
    void put(const std::string& name, const T& d)
    {
        writeIndent();
        std::cout << name << " = " << d << std::endl;
    }

    COutOutputStream()
        : indent(0)
    {
    }

    void writeIndent()
    {
        std::cout << std::string(indent, ' ');
    }

    size_t indent;
};

template <>
void COutOutputStream::put<StructureStart>(const std::string& name, const StructureStart& d)
{
    writeIndent();
    indent += 2;
    std::cout << name << " {" << std::endl;
}

template <>
void COutOutputStream::put<StructureEnd>(const std::string& name, const StructureEnd& d)
{
    indent = std::max((size_t) 0, indent-2);
    writeIndent();
    std::cout << "}" << std::endl;
}

};

