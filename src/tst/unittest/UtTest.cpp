//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Test Cases
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtTest);

//------------------------------------------------------------------------------
TEST(give_X50Y20_when_DecomposeXOpY_then_StringValid)
{
    int x = 50;
    int y = 20;

    TestAssert::Decomposer d;

    d >= x > y;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "50 > 20");

    d >= x < y;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "50 < 20");

    d >= x >= y;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "50 >= 20");

    d >= x <= y;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "50 <= 20");

    d >= x == y;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "50 == 20");

    d >= x != y;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "50 != 20");
}

//------------------------------------------------------------------------------
TEST(give_X50Y20_when_DecomposeBracketedXOpY_then_StringTrueFalse)
{
    int x = 50;
    int y = 20;

    TestAssert::Decomposer d;

    d >= (x > y);
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "true");

    d >= (x <= y);
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "false");
}

//------------------------------------------------------------------------------
TEST(give_BoolTest_when_Decompose_then_StringTrueFalse)
{
    bool b = true;
    bool f = false;

    TestAssert::Decomposer d;

    d >= b;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "true");

    d >= !b;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "false");

    d >= f;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "false");

    d >= !f;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "true");

    d >= b == f;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "true == false");

    d >= b != f;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "true != false");
}

//------------------------------------------------------------------------------
static int NonReentrantValue = 30;
static int NonReentrantFunction()
{
    int x = NonReentrantValue;
    NonReentrantValue += 1;
    return x;
}
TEST(give_NonReentrantFunction_when_RequireOrCheck_then_FunctionCalledOnce)
{
    int y = 31;

    NonReentrantValue = 30;

    CHECK(NonReentrantFunction() < y);
    REQUIRE(NonReentrantValue == 31);

    REQUIRE(y == NonReentrantFunction());
    REQUIRE(NonReentrantValue == 32);

    CHECK(NonReentrantFunction() > y);
    REQUIRE(NonReentrantValue == 33);
}




//=============================== End of File ==================================
