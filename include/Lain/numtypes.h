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

#include "Structure.h"

namespace Lain {

    namespace detail {

        template<int SIZE>
        class IntegerTraits {
            // TODO static assert SIZE <= 64
            typedef typename IntegerTraits<SIZE+1>::SignedFieldType SignedFieldType;
            typedef typename IntegerTraits<SIZE+1>::UnsignedFieldType UnsignedFieldType;
        };

        template<>
        class IntegerTraits<8> {
            typedef char SignedFieldType;
            typedef unsigned char UnsignedFieldType;
        };

        template<>
        class IntegerTraits<16> {
            typedef short SignedFieldType;
            typedef unsigned short UnsignedFieldType;
        };

        template<>
        class IntegerTraits<32> {
            typedef long SignedFieldType;
            typedef unsigned long UnsignedFieldType;
        };

        template<>
        class IntegerTraits<64> {
            typedef long SignedFieldType;
            typedef unsigned long UnsignedFieldType;
        };

    }

    template <typename C, int SIZE>
    class SignedInt : public Field<C, typename detail::IntegerTraits<SIZE>::SignedFieldType>
    {
        public:
            SignedInt() : Field<C, typename detail::IntegerTraits<SIZE>::SignedFieldType>() {}
    };

    template <typename C, int SIZE>
    class UnsignedInt : public Field<C, typename detail::IntegerTraits<SIZE>::UnsignedFieldType>
    {
        public:
            UnsignedInt() : Field<C, typename detail::IntegerTraits<SIZE>::UnsignedFieldType>() {}
    };


};
