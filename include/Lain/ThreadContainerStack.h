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

class ContainerBase;

namespace internal {

auto& getThreadContainerStack()
{
    static thread_local std::stack<ContainerBase*> _stack;
    return _stack;
}

};
};

