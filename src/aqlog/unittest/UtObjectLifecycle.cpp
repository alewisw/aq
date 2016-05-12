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

#include "TestHandler.h"




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
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtObjectLifecycle);

//------------------------------------------------------------------------------
TEST(given_AQLogFilter_when_CopyConstructed_then_ObjectsIdentical)
{
    TestHandler h;
    AQLogFilter a(h, AQLOG_LEVEL_WARNING, "foo", "bar", "baz");
    AQLogFilter b(a);

    REQUIRE(&b.handler() == &h);
    REQUIRE(b.level() == AQLOG_LEVEL_WARNING);
    REQUIRE(b.tierIdCount() == 3);
    REQUIRE(b.tierId(0) == "foo");
    REQUIRE(b.tierId(1) == "bar");
    REQUIRE(b.tierId(2) == "baz");
}

//------------------------------------------------------------------------------
TEST(given_AQLogFilter_when_Assigned_then_ObjectsIdentical)
{
    TestHandler ha;
    TestHandler hb;
    AQLogFilter a(ha, AQLOG_LEVEL_WARNING, "foz", "biz", "");
    AQLogFilter b(hb, AQLOG_LEVEL_ERROR, "foo", "bar", "baz");

    b = a;

    REQUIRE(&b.handler() == &ha);
    REQUIRE(b.level() == AQLOG_LEVEL_WARNING);
    REQUIRE(b.tierIdCount() == 2);
    REQUIRE(b.tierId(0) == "foz");
    REQUIRE(b.tierId(1) == "biz");
}



//=============================== End of File ==================================
