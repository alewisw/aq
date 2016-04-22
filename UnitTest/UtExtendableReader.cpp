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
#include "TestPointAction.h"




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
TEST_SUITE(UtExtendableReader);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_Item1CommitInterrupted_when_Item1CommitComplete_then_Item2Item1Retrieved, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem0, witem1;
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.appendData(witem0, 0, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize()));
    REQUIRE(aq.appendData(witem1, 10, 2 * aq.pageSize()));
    REQUIRE(aq.appendData(witem0, aq.pageSize(), aq.pageSize()));

    REQUIRE(aq.writer.commitExtendable(witem0, 0, 0));
    REQUIRE(aq.writer.commit(witem1));

    AQItem ritem1;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.isItemData(ritem1, 10, 2 * aq.pageSize()));
    aq.reader.release(ritem1);

    REQUIRE(aq.writer.commitExtendable(witem0, 1, 1));

    AQItem ritem0;
    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(aq.isItemData(ritem0, 0, 2 * aq.pageSize()));
    aq.reader.release(ritem0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_LargeItem1CommitInterruptedMultipeTimes_when_Item1CommitComplete_then_Item2Item1Retrieved, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem0, witem1, witem2;
    AQItem ritem0, ritem1, ritem2;
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.appendData(witem0, 0, aq.pageSize()));               // Item 0: [p1]

    REQUIRE(aq.writer.claim(witem1, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem2, aq.pageSize()));

    REQUIRE(aq.appendData(witem1, 10, 2 * aq.pageSize()));          // Item 1: [p1]->[p2]

    REQUIRE(aq.appendData(witem0, aq.pageSize(), aq.pageSize()));       // Item 0: [p1]->[p2]
    REQUIRE(aq.appendData(witem2, 20, aq.pageSize()));              // Item 2: [p1]
    REQUIRE(aq.appendData(witem0, 2 * aq.pageSize(), aq.pageSize()));   // Item 0: [p1]->[p2]->[p3]
    REQUIRE(aq.writer.commitExtendable(witem0, 0, 0));          // Item 0: {P1}->[p2]->[p3]
    REQUIRE(!aq.reader.retrieve(ritem0));                    // Cannot retrieve partial.

    REQUIRE(aq.writer.commitExtendable(witem1, 0, 0));          // Item 1: {P1}->[p2]
    REQUIRE(!aq.reader.retrieve(ritem1));                    // Cannot retrieve partial.

    REQUIRE(aq.appendData(witem2, 20 + aq.pageSize(), aq.pageSize()));  // Item 2: [p1]->[p2]
    REQUIRE(aq.appendData(witem2, 20 + 2*aq.pageSize(), aq.pageSize()));// Item 2: [p1]->[p2]->[p3]

    REQUIRE(aq.writer.commitExtendable(witem2, 0, 1));          // Item 2: {P1}->{P2}->[p3]
    REQUIRE(!aq.reader.retrieve(ritem2));                    // Cannot retrieve partial.

    REQUIRE(aq.writer.commitExtendable(witem1, 1, 1));          // Item 1: {P1}->{P2}
    REQUIRE(aq.reader.retrieve(ritem1));                     // Retrieve complete.
    REQUIRE(aq.isItemData(ritem1, 10, 2 * aq.pageSize()));
    aq.reader.release(ritem1);

    REQUIRE(aq.writer.commitExtendable(witem2, 2, 2));          // Item 2: {P1}->{P2}->{P3}
    REQUIRE(aq.writer.commitExtendable(witem0, 1, 2));          // Item 0: {P1}->{P2}->{P3}
    REQUIRE(aq.reader.retrieve(ritem0));                     // Retrieve complete.
    REQUIRE(aq.isItemData(ritem0, 0, 3 * aq.pageSize()));
    REQUIRE(aq.reader.retrieve(ritem2));                     // Retrieve complete.
    REQUIRE(aq.isItemData(ritem2, 20, 3 * aq.pageSize()));
    aq.reader.release(ritem0);
    aq.reader.release(ritem2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CrashDuringCommit_when_Timeout_then_IncompleteItemRetrieved, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem0, witem1;
    AQItem ritem0;

    // Allocate and commit the first page, but don't commit the rest.
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.appendData(witem0, 0, 2 * aq.pageSize()));
    REQUIRE(aq.writer.commitExtendable(witem0, 0, 0));

    // Verify - cannot retrieve partial - also start the clock.
    REQUIRE(!aq.reader.retrieve(ritem0));

    // Run the timeout, then allocate some more data in the queue to
    // make the incomplete logic trigger.
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.writer.claim(witem1, (aq.pageCount() - 4) * aq.pageSize()));

    // Now retrieve the original item - it should come back as incomplete.
    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(!ritem0.isCommitted());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CrashBeforeCommit_when_Timeout_then_ItemDiscarded, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem0, witem1, witem2;
    AQItem ritem0;
    
    // Make the index '0' clear.
    aq.advance(1);

    // Allocate the item but never commit it.  This consumes index '1' and '2'.
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.appendData(witem0, 0, 2 * aq.pageSize()));

    // Verify - cannot retrieve partial - also start the clock.
    REQUIRE(!aq.reader.retrieve(ritem0));

    // Run the timeout, then allocate some more data in the queue to
    // make the incomplete logic trigger.
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.writer.claim(witem1, (aq.pageCount() - 3) * aq.pageSize()));

    REQUIRE(aq.reader.availableSize() == 0);
    REQUIRE(!aq.writer.claim(witem2, aq.pageSize()));

    // Now try to retrieve the original item - it should fail but space becomes
    // available in the queue.
    REQUIRE(!aq.reader.retrieve(ritem0));

    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
    REQUIRE(aq.writer.claim(witem2, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMidItemInterruptsRetrieve_when_Retrieve_then_ItemRetrieved, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem;
    AQItem ritem;

    // Allocate the item but never commit it.  This consumes index '1' and '2'.
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, 1 * aq.pageSize()));
    REQUIRE(aq.appendData(witem, 1 * aq.pageSize(), 1 * aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 1 * aq.pageSize()));

    CommitAction action(aq.writer, witem);
    action.attach(aq.reader, AQReader::WalkAfterReadCtrlN + 0);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 3 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitLastOfTwoItemsInterruptsRetrieve_when_Retrieve_then_ItemRetrieved, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem;
    AQItem ritem;

    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, 1 * aq.pageSize()));
    REQUIRE(aq.appendData(witem, 1 * aq.pageSize(), 1 * aq.pageSize()));

    CommitAction action(aq.writer, witem);
    action.attach(aq.reader, AQReader::WalkAfterReadCtrlN + 0);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitLastOfThreeItemsInterruptsRetrieve_when_Retrieve_then_ItemRetrieved, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem;
    AQItem ritem;

    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, 1 * aq.pageSize()));
    REQUIRE(aq.appendData(witem, 1 * aq.pageSize(), 1 * aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 1 * aq.pageSize()));

    CommitAction action(aq.writer, witem);
    action.attach(aq.reader, AQReader::WalkAfterReadCtrlN + 1);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 3 * aq.pageSize()));
}




//=============================== End of File ==================================
