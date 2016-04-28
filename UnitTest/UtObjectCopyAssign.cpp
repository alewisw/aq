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

#include "AQTest.h"

#include <cstring>




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Builds our reference AQWriterItem for copy tests consisting of 'itemCount'
// linked items.
static void buildWriterItem(AQTest& aq, AQWriterItem& item, size_t itemCount);

// Builds our reference AQItem for copy tests consisting of 'itemCount'
// linked items.
static void buildItem(AQTest& aq, AQItem& item, size_t itemCount);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtObjectCopyAssign);

//------------------------------------------------------------------------------
TEST(given_AQUnformattedException_when_CopyConstructed_then_ObjectsIdentical)
{
    AQUnformattedException a("oops");
    AQUnformattedException b(a);

    REQUIRE(strcmp(a.what(), b.what()) == 0);
}

//------------------------------------------------------------------------------
TEST(given_AQUnformattedException_when_Assigned_then_ObjectsIdentical)
{
    AQUnformattedException a("foo");
    AQUnformattedException b("bar");

    b = a;

    REQUIRE(strcmp(a.what(), b.what()) == 0);
}

//------------------------------------------------------------------------------
TEST(given_AQWriter_when_CopyConstructed_then_ObjectsIdentical)
{
    unsigned char mem[1000];
    AQWriter a(mem, sizeof(mem));
    AQWriter b(a);

    REQUIRE(a.memorySize() == b.memorySize());

    AQReader r(mem, sizeof(mem));
    r.format(2, 1000, AQ::OPTION_EXTENDABLE);

    REQUIRE(a.isExtendable());
    REQUIRE(b.isExtendable());

    r.format(2, 1000, 0);

    REQUIRE(!a.isExtendable());
    REQUIRE(!b.isExtendable());
}

//------------------------------------------------------------------------------
TEST(given_AQWriter_when_Assigned_then_ObjectsIdentical)
{
    unsigned char mem[1000];
    unsigned char memb[500];
    AQWriter a(mem, sizeof(mem));
    AQWriter b(memb, sizeof(memb));

    b = a;

    REQUIRE(a.memorySize() == b.memorySize());

    AQReader r(mem, sizeof(mem));
    r.format(2, 1000, AQ::OPTION_EXTENDABLE);

    REQUIRE(a.isExtendable());
    REQUIRE(b.isExtendable());

    r.format(2, 1000, 0);

    REQUIRE(!a.isExtendable());
    REQUIRE(!b.isExtendable());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQWriterItem_when_FirstCopyConstructed_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem a;
    buildWriterItem(aq, a, 3);
    AQWriterItem b(a);
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_OneItemAQItem_when_FirstCopyConstructed_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 1);
    AQItem b(a);
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoItemAQItem_when_FirstCopyConstructed_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 2);
    AQItem b(a);
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQItem_when_FirstCopyConstructed_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 3);
    AQItem b(a);
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQWriterItem_when_FirstAssignedOverOneItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem a;
    buildWriterItem(aq, a, 3);
    AQWriterItem b;
    buildWriterItem(aq, b, 1);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQItem_when_FirstAssignedOverOneItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 3);
    AQItem b;
    buildItem(aq, b, 1);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQItem_when_FirstAssignedOverTwoItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 3);
    AQItem b;
    buildItem(aq, b, 2);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQItem_when_FirstAssignedOverThreeItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 3);
    AQItem b;
    buildItem(aq, b, 3);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQItem_when_FirstAssignedOverFourItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 3);
    AQItem b;
    buildItem(aq, b, 4);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeItemAQItem_when_FirstAssignedOverFiveItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 3);
    AQItem b;
    buildItem(aq, b, 5);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_OneItemAQItem_when_FirstAssignedOverOneItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 1);
    AQItem b;
    buildItem(aq, b, 1);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_OneItemAQItem_when_FirstAssignedOverTwoItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 1);
    AQItem b;
    buildItem(aq, b, 2);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_OneItemAQItem_when_FirstAssignedOverThreeItem_then_ObjectsIdentical, AQ::OPTION_EXTENDABLE)
{
    AQItem a;
    buildItem(aq, a, 1);
    AQItem b;
    buildItem(aq, b, 3);
    b = a;
    REQUIRE(aq.areFullyIdenticalAllocatedItems(a, b));
}

//------------------------------------------------------------------------------
static void buildWriterItem(AQTest& aq, AQWriterItem& item, size_t itemCount)
{
    CHECK(aq.writer.claim(item, 1));

    for (size_t i = 0; i < itemCount; ++i)
    {
        CHECK(aq.appendData(item, i * aq.pageSize(), 
            i == itemCount - 1 ? (aq.pageSize() / 2) : aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
static void buildItem(AQTest& aq, AQItem& item, size_t itemCount)
{
    AQWriterItem witem;
    buildWriterItem(aq, witem, itemCount);
    CHECK(aq.writer.commit(witem));

    CHECK(aq.reader.retrieve(item));
}



//=============================== End of File ==================================
