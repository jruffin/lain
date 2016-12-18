/*
 * Serialization Experiments Lain
 *
 * Copyright (C) 2016 Julien Ruffin and project contributors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lain/Structure.h>
#include <lain/Context.h>
#include <lain/COutOutputStream.h>
#include <lain/StringInputStream.h>
#include <lain/PackedBinaryStreams.h>
#include <lain/Field.h>
#include <lain/Constant.h>

#include <cstdint>
#include <vector>
#include <iterator>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

template <typename C>
struct Foo
{
    lain::Field<C, int> a = {"a"};
    lain::Field<C, std::string> text = {"text"};
};

struct Point
{
    int x;
    int y;
};

namespace lain {
    template <>
    void COutOutputStream::put<Point>(const std::string& name, const Point& p)
    {
        writeIndent();
        std::cout << name << " = (" << p.x << "," << p.y << ")" << std::endl;
    }
}

template <typename C>
struct Bar
{
    lain::Field<C, std::string> before = {"before"};
    lain::Structure<C, Foo> foo = {"foo"};
    lain::Field<C, std::string> after = {"after"};
    lain::Field<C, Point> testPoint = {"testPoint"};
};


TEST_CASE("Simple structure")
{
    typedef lain::MakeContext<
        lain::NullInputStream,
        lain::COutOutputStream
            > Context;

    lain::Structure<Context, Foo> a("foo");

    a.a.value = 2;
    a.text.value = "Some text";

    lain::COutOutputStream s;
    a.write(s);

    lain::Structure<Context, Bar> b("bar");

    b.before.value = "before";
    b.after.value = "after";
    b.foo.a.value = 4;
    b.foo.text.value = "text";
    b.testPoint.value.x = 42;
    b.testPoint.value.y = 21;

    b.write(s);
}

template <typename C>
struct SimpleStructure2
{
    lain::Field<C, int32_t> a;
    lain::Field<C, int8_t> b;
    lain::Field<C, int16_t> c;
};

TEST_CASE("Simple Structure 2")
{
    typedef std::back_insert_iterator< std::vector<char> > VectorBackInserter;
    typedef std::vector<char>::const_iterator VectorConstIterator;

    typedef lain::MakeContext<
        lain::PackedBinaryInputStream<VectorConstIterator>,
        lain::PackedBinaryOutputStream<VectorBackInserter> > Context;

    lain::Structure<Context, SimpleStructure2> s1;

    SECTION ("Packed binary write")
    {
        s1.a.value = 111;
        s1.b.value = 222;
        s1.c.value = 333;

        std::vector<char> output;
        lain::PackedBinaryOutputStream<VectorBackInserter> os(std::back_inserter(output));
        s1.write(os);

        // Expected contents, all little-endian:
        // 4 bytes A | 1 byte B | 2 bytes C
        // 6F 00 00 00 DE 4D 01
        std::vector<char> expected = { 0x6F, 0, 0, 0, (char) 0xDE, 0x4D, 0x01 };

        REQUIRE(output == expected);
    }

    SECTION ("Packed binary read")
    {
        // Expected contents, all little-endian:
        // 4 bytes A | 1 byte B | 2 bytes C
        //   6521315 |       98 |      9265
        //  E3 81 63 |       62 |     31 24
        std::vector<char> input = { (char) 0xE3, (char) 0x81, 0x63, 0x00, 0x62, 0x31, 0x24 };

        lain::PackedBinaryInputStream<VectorConstIterator> is(input.cbegin(), input.cend());

        s1.read(is);

        REQUIRE(s1.a.value == 6521315);
        REQUIRE(s1.b.value == 98);
        REQUIRE(s1.c.value == 9265);
    }

    SECTION ("Not enough bytes to read")
    {
        // This is too short to be entirely read, so it must fail
        std::vector<char> input = { (char) 0xE3, (char) 0x81, 0x63, 0x00 };

        lain::PackedBinaryInputStream<VectorConstIterator> is(input.cbegin(), input.cend());

        REQUIRE_THROWS_AS(s1.read(is), std::runtime_error);
    }

}

template <typename C>
struct StructWithConstant
{
    lain::Constant<C, int> version = {"version", 20};
    lain::Field<C, int> length = {"length", 0};
};


TEST_CASE("Struct with constant")
{
    typedef lain::MakeContext<
        lain::StringInputStream,
        lain::COutOutputStream
            > Context;

    lain::Structure<Context, StructWithConstant> s = {"s"};

    SECTION ("Writing")
    {
        lain::COutOutputStream os;
        s.write(os);
    }

    SECTION ("Reading with valid constant value")
    {
        lain::StringInputStream is("20 999");
        s.read(is);
        REQUIRE(s.version.value == 20);
        REQUIRE(s.length.value == 999);
    }

    SECTION ("Reading with invalid constant value")
    {
        lain::StringInputStream is("-42 999");
        REQUIRE_THROWS_AS(s.read(is), std::runtime_error);
    }
}


/*
 * Idea for later
template <typename C>
struct TestHeader
{
    lain::NumConstant<C, int, 2> version; // Version should be 2
};
*/
