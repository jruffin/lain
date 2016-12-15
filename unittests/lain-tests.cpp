#include <lain/Structure.h>
#include <lain/Context.h>
#include <lain/COutOutputStream.h>
#include <lain/StringInputStream.h>
#include <lain/Field.h>
#include <lain/Constant.h>
#include <lain/numtypes.h>

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
