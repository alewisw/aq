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

#include <stdint.h>




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
TEST(given_X50Y20_when_DecomposeXOpY_then_StringValid)
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
TEST(given_X50Y20_when_DecomposeBracketedXOpY_then_StringOneZero)
{
    int x = 50;
    int y = 20;

    TestAssert::Decomposer d;

    d >= (x > y);
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "1");

    d >= (x < y);
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "0");
}

//------------------------------------------------------------------------------
TEST(given_BoolTest_when_Decompose_then_StringOneZero)
{
    bool b = true;
    bool f = false;

    TestAssert::Decomposer d;

    d >= b;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "1");

    d >= !b;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "0");

    d >= b == f;
    REQUIRE(!d.outcome());
    REQUIRE(d.str() == "1 == 0");

    d >= b != f;
    REQUIRE(d.outcome());
    REQUIRE(d.str() == "1 != 0");
}

//------------------------------------------------------------------------------
TEST(given_PrimitiveTest_when_Decompose_then_StringPrimitiveValue)
{
    TestAssert::Decomposer d;

    { static volatile signed char v = 0x35; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "5"); }
    { static volatile signed char v = 0; d >= v; REQUIRE(!d.outcome()); REQUIRE(d.str() == ""); }
    { static volatile unsigned char v = 36; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "$"); }
    { static volatile unsigned char v = 0; d >= v; REQUIRE(!d.outcome());  REQUIRE(d.str() == ""); }

    { static volatile signed short v = -5; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "-5"); }
    { static volatile signed short v = 0; d >= v; REQUIRE(!d.outcome()); REQUIRE(d.str() == "0"); }
    { static volatile unsigned short v = 35; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "35"); }
    { static volatile unsigned short v = 0; d >= v; REQUIRE(!d.outcome());  REQUIRE(d.str() == "0"); }

    { static volatile signed int v = -5; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "-5"); }
    { static volatile signed int v = 0; d >= v; REQUIRE(!d.outcome()); REQUIRE(d.str() == "0"); }
    { static volatile unsigned int v = 35; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "35"); }
    { static volatile unsigned int v = 0; d >= v; REQUIRE(!d.outcome());  REQUIRE(d.str() == "0"); }

    { static volatile signed long v = -5; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "-5"); }
    { static volatile signed long v = 0; d >= v; REQUIRE(!d.outcome()); REQUIRE(d.str() == "0"); }
    { static volatile unsigned long v = 35; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "35"); }
    { static volatile unsigned long v = 0; d >= v; REQUIRE(!d.outcome());  REQUIRE(d.str() == "0"); }

    { static volatile int64_t v = -5; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "-5"); }
    { static volatile int64_t v = 0; d >= v; REQUIRE(!d.outcome()); REQUIRE(d.str() == "0"); }
    { static volatile uint64_t v = 35; d >= v; REQUIRE(d.outcome());  REQUIRE(d.str() == "35"); }
    { static volatile uint64_t v = 0; d >= v; REQUIRE(!d.outcome());  REQUIRE(d.str() == "0"); }
}

//------------------------------------------------------------------------------
static int NonReentrantValue = 30;
static int NonReentrantFunction()
{
    int x = NonReentrantValue;
    NonReentrantValue += 1;
    return x;
}
TEST(given_NonReentrantFunction_when_RequireOrCheck_then_FunctionCalledOnce)
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
