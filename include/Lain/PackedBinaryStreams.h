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

    class StructureStart;
    class StructureEnd;

    template <typename iterator_type>
    class PackedBinaryInputStream : public AbstractInputStream
    {
        public:
            PackedBinaryInputStream(iterator_type start, iterator_type end)
                : it(start), end(end) {}

            template <typename T>
            void get(const std::string& name, T& data)
            {
                union coercion { T t; char bytes[sizeof(T)]; };
                coercion c = {0};


                for (size_t i=0; i < sizeof(T); ++i) {
                    checkIterator();
                    c.bytes[i] = *it++;
                }
                data = c.t;
            }

            void checkIterator()
            {
                if(it >= end) {
                    throw std::runtime_error("Attempt to read past the end");
                }
            }

            // Ignore framing since we have a packed format
            void get(const std::string& name, StructureStart& s) {}
            void get(const std::string& name, StructureEnd& s) {}

        private:
            iterator_type it;
            iterator_type end;
    };

    template <typename insert_iterator_type>
    class PackedBinaryOutputStream : public AbstractOutputStream
    {
        public:
            PackedBinaryOutputStream(insert_iterator_type it) : it(it) {}

            template <typename T>
            void put(const std::string& name, const T& data)
            {
                union coercion { T t; char bytes[sizeof(T)]; };
                coercion c = {0};
                c.t = data;

                for (size_t i=0; i < sizeof(T); ++i) {
                    *it++ = c.bytes[i];
                }
            }

            // Ignore framing since we have a packed format
            void put(const std::string& name, const StructureStart& s) {}
            void put(const std::string& name, const StructureEnd& s) {}

        private:
            insert_iterator_type it;
    };
};

