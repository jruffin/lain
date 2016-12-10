#include <lain/structure.h>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

template <typename C>
struct Foo
{
    lain::Field<C, int> a;
    lain::Field<C, std::string> text;
};

template <typename C>
struct Bar
{
    lain::Field<C, std::string> before;
    lain::Structure<C, Foo> foo;
    lain::Field<C, std::string> after;
};

struct SomeOtherSerializer {};

TEST_CASE("Simple structure")
{
    typedef lain::MakeContext<lain::COutSerializer> Context;

    lain::Structure<Context, Foo> a;

    a.a.value = 2;
    a.text.value = "Some text";

    lain::COutSerializer s;
    a.write(s);

    lain::Structure<Context, Bar> b;
    b.before.value = "before";
    b.after.value = "after";
    b.foo.a.value = 4;
    b.foo.text.value = "text";

    b.write(s);
}

