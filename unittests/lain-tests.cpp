#include <lain/structure.h>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

struct Foo : public lain::Structure<lain::COutSerializer>
{
    Foo() {
        std::cout << "init done" << std::endl;
        _lain_initDone();
    }

    lain::Field<C, int> a;
    lain::Field<C, std::string> text;
};

TEST_CASE("Simple structure")
{
    Foo a;
    a.a.value = 2;
    a.text.value = "Some text";

    lain::COutSerializer s;
    a.write(s);
}

