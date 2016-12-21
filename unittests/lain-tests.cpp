/*
 * Serialization Experiments Lain
 *
 * Copyright (C) 2016 Julien Ruffin and project contributors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <Lain/Structure.h>
#include <Lain/Context.h>
#include <Lain/COutOutputStream.h>
#include <Lain/StringInputStream.h>
#include <Lain/PackedBinaryStreams.h>
#include <Lain/Field.h>
#include <Lain/Constant.h>

#include <cstdint>
#include <vector>
#include <iterator>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

template <typename C>
struct Foo
{
    Lain::SimpleField<C, int> a = {"a"};
    Lain::SimpleField<C, std::string> text = {"text"};
};

struct Point
{
    int x;
    int y;
};

namespace Lain {
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
    Lain::SimpleField<C, std::string> before = {"before"};
    Lain::Structure<C, Foo> foo = {"foo"};
    Lain::SimpleField<C, std::string> after = {"after"};
    Lain::SimpleField<C, Point> testPoint = {"testPoint"};
};


TEST_CASE("Simple structure")
{
    typedef Lain::MakeContext<
        Lain::NullInputStream,
        Lain::COutOutputStream
            > Context;

    Lain::Structure<Context, Foo> a("foo");

    a.a.value = 2;
    a.text.value = "Some text";

    Lain::COutOutputStream s;
    a.write(s);

    Lain::Structure<Context, Bar> b("bar");

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
    Lain::SimpleField<C, int32_t> a = {"a"};
    Lain::SimpleField<C, int8_t> b = {"b"};
    Lain::SimpleField<C, int16_t> c = {"c"};
};

TEST_CASE("Simple Structure 2")
{
    typedef std::back_insert_iterator< std::vector<char> > VectorBackInserter;
    typedef std::vector<char>::const_iterator VectorConstIterator;

    typedef Lain::MakeContext<
        Lain::PackedBinaryInputStream<VectorConstIterator>,
        Lain::PackedBinaryOutputStream<VectorBackInserter> > Context;

    Lain::Structure<Context, SimpleStructure2> s1;

    SECTION ("Packed binary write")
    {
        s1.a.value = 111;
        s1.b.value = 222;
        s1.c.value = 333;

        std::vector<char> output;
        Lain::PackedBinaryOutputStream<VectorBackInserter> os(std::back_inserter(output));
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

        Lain::PackedBinaryInputStream<VectorConstIterator> is(input.cbegin(), input.cend());

        s1.read(is);

        REQUIRE(s1.a.value == 6521315);
        REQUIRE(s1.b.value == 98);
        REQUIRE(s1.c.value == 9265);
    }

    SECTION ("Not enough bytes to read")
    {
        // This is too short to be entirely read, so it must fail
        std::vector<char> input = { (char) 0xE3, (char) 0x81, 0x63, 0x00 };

        Lain::PackedBinaryInputStream<VectorConstIterator> is(input.cbegin(), input.cend());

        REQUIRE_THROWS_AS(s1.read(is), std::runtime_error);
    }

}

template <typename C>
struct StructWithConstant
{
    Lain::Constant<C, int> version = {"version", 20};
    Lain::SimpleField<C, int> length = {"length", 0};
};


TEST_CASE("Struct with constant")
{
    typedef Lain::MakeContext<
        Lain::StringInputStream,
        Lain::COutOutputStream
            > Context;

    Lain::Structure<Context, StructWithConstant> s = {"s"};

    SECTION ("Writing")
    {
        Lain::COutOutputStream os;
        s.write(os);
    }

    SECTION ("Reading with valid constant value")
    {
        Lain::StringInputStream is("20 999");
        s.read(is);
        REQUIRE(s.version.value == 20);
        REQUIRE(s.length.value == 999);
    }

    SECTION ("Reading with invalid constant value")
    {
        Lain::StringInputStream is("-42 999");
        REQUIRE_THROWS_AS(s.read(is), std::runtime_error);
    }
}

TEST_CASE("Structure copying and moving")
{
    typedef Lain::MakeContext<
        Lain::NullInputStream,
        Lain::NullOutputStream
            > Context;

    SECTION ("Structure copying")
    {
        Lain::Structure<Context, SimpleStructure2> t = {"t"};
        t.a.value = 1;
        t.b.value = 2;
        t.c.value = 3;

        SECTION ("Copy constructor")
        {
            Lain::Structure<Context, SimpleStructure2> s(t);

            REQUIRE(s.getName() == t.getName());
            REQUIRE(s.a.value == t.a.value);
            REQUIRE(s.b.value == t.b.value);
            REQUIRE(s.c.value == t.c.value);
        }

        SECTION ("Copy assignment")
        {
            Lain::Structure<Context, SimpleStructure2> s = {"s"};
            s = t;

            REQUIRE(s.getName() == t.getName());
            REQUIRE(s.a.value == t.a.value);
            REQUIRE(s.b.value == t.b.value);
            REQUIRE(s.c.value == t.c.value);
        }
    }

    /*
    SECTION ("Structure moving")
    {

        auto f = []() {
            Lain::Structure<Context, SimpleStructure2> t = {"t"};
            t.a.value = 1;
            t.b.value = 2;
            t.c.value = 3;
            return t;
        };

        SECTION ("Move constructor")
        {
            // The abusive std::move is here to avoid using RVA
            Lain::Structure<Context, SimpleStructure2> s(std::move(f()));

            REQUIRE(s.a.value == 1);
            REQUIRE(s.b.value == 2);
            REQUIRE(s.c.value == 3);
        }

        SECTION ("Move assignment")
        {
            Lain::Structure<Context, SimpleStructure2> s = {"s"};
            s = f();

            REQUIRE(s.a.value == 1);
            REQUIRE(s.b.value == 2);
            REQUIRE(s.c.value == 3);
        }
    }
    */

}

TEST_CASE("Context switch")
{
    typedef Lain::MakeContext<
        Lain::NullInputStream,
        Lain::NullOutputStream
            > Context1;

    typedef Lain::MakeContext<
        Lain::NullInputStream,
        Lain::COutOutputStream
            > Context2;

    SECTION ("Simple structure")
    {
        Lain::Structure<Context1, SimpleStructure2> s1;

        s1.a.value = 1;
        s1.b.value = 2;
        s1.c.value = 3;

        SECTION ("Per copy constructor")
        {
            Lain::Structure<Context2, SimpleStructure2> s2(s1);

            REQUIRE(s2.a.value == s1.a.value);
            REQUIRE(s2.b.value == s1.b.value);
            REQUIRE(s2.c.value == s1.c.value);
        }

        SECTION ("Per assignment operator")
        {
            Lain::Structure<Context2, SimpleStructure2> s2;

            s2 = s1;

            REQUIRE(s2.a.value == s1.a.value);
            REQUIRE(s2.b.value == s1.b.value);
            REQUIRE(s2.c.value == s1.c.value);
        }
    }
}

