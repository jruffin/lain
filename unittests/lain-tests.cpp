#include <lain/structure.h>
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
struct SomeHeader
{
    lain::SignedInt<C, 15> foo;
    lain::SignedInt<C, 15> bar;
};


TEST_CASE("Some header")
{
}


/*
 * Idea for later
template <typename C>
struct TestHeader
{
    lain::NumConstant<C, int, 2> version; // Version should be 2
};
*/
