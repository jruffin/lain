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

#include "AbstractStream.h"

namespace Lain {

struct AbstractField
{
    virtual ~AbstractField() {}

    virtual const std::string& getName() const = 0;

    virtual void read(AbstractInputStream&) = 0;
    virtual void write(AbstractOutputStream&) = 0;
};

};
