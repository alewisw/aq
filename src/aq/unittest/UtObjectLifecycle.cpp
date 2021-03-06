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

#include <string.h>




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
TEST_SUITE(UtObjectLifecycle);

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
    AQUnformattedException *a = new AQUnformattedException("foo");
    AQUnformattedException b("bar");

    b = *a;

    REQUIRE(strcmp(a->what(), b.what()) == 0);

    delete a;
}

//------------------------------------------------------------------------------
TEST(given_AQUnformattedException_when_SelfAssigned_then_ObjectUnchanged)
{
    AQUnformattedException a("foo");
    AQUnformattedException b("bar");

    b = a;
    a = a;

    REQUIRE(strcmp(a.what(), b.what()) == 0);
}

//------------------------------------------------------------------------------
TEST(given_AQWriter_when_CopyConstructed_then_ObjectsIdentical)
{
    AQHeapMemory sm(1000);
    AQWriter a(sm);
    AQWriter b(a);

    REQUIRE(a.memorySize() == b.memorySize());

    AQReader r(sm);
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
    AQHeapMemory sm(1000);
    AQHeapMemory smb(500);
    AQWriter a(sm);
    AQWriter *b = new AQWriter(smb);

    *b = a;

    REQUIRE(a.memorySize() == b->memorySize());

    AQReader r(sm);
    r.format(2, 1000, AQ::OPTION_EXTENDABLE);

    REQUIRE(a.isExtendable());
    REQUIRE(b->isExtendable());

    r.format(2, 1000, 0);

    REQUIRE(!a.isExtendable());
    REQUIRE(!b->isExtendable());

    delete b;
}

//------------------------------------------------------------------------------
TEST(given_AQWriter_when_SelfAssigned_then_ObjectUnchanged)
{
    AQHeapMemory sm(100);
    AQHeapMemory smb(500);

    AQWriter a(sm);
    AQWriter b(smb);

    b = a;
    a = a;

    REQUIRE(a.memorySize() == b.memorySize());
}

//------------------------------------------------------------------------------
TEST(given_NewAQWriter_when_Deleted_then_ObjectDestroyed)
{
    AQHeapMemory sm(1000);
    AQReader *a = new AQReader(sm);
    delete a;
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
