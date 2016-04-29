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
TEST_SUITE(UtRetrieve);

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAtFirstPage_when_Retrieve_then_FailsWithNullItem)
{
    AQItem ritem;
    REQUIRE(!aq.reader.retrieve(ritem));
    REQUIRE(!ritem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAtMidPage_when_Retrieve_then_FailsWithNullItem)
{
    aq.advance(7);
    AQItem ritem;
    REQUIRE(!aq.reader.retrieve(ritem));
    REQUIRE(!ritem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAtLastPage_when_Retrieve_then_FailsWithNullItem)
{
    aq.advance(10);
    AQItem ritem;
    REQUIRE(!aq.reader.retrieve(ritem));
    REQUIRE(!ritem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_CommitAt1At4At3_when_RetrieveCommitAt0At2Retrieve_then_Order13402)
{
    AQWriterItem witem0;
    AQWriterItem witem1;
    AQWriterItem witem2;
    AQWriterItem witem3;
    AQWriterItem witem4;
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem2, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem3, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem4, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.writer.commit(witem4));
    REQUIRE(aq.writer.commit(witem3));

    AQItem ritem;
    AQItem ritem0;
    AQItem ritem1;
    AQItem ritem2;
    AQItem ritem3;
    AQItem ritem4;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem3));
    REQUIRE(aq.reader.retrieve(ritem4));
    REQUIRE(!aq.reader.retrieve(ritem));

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.writer.commit(witem2));

    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(!aq.reader.retrieve(ritem));

    REQUIRE(aq.isItemPage(ritem0, 0, aq.pageSize()));
    REQUIRE(aq.isItemPage(ritem1, 1, aq.pageSize()));
    REQUIRE(aq.isItemPage(ritem2, 2, aq.pageSize()));
    REQUIRE(aq.isItemPage(ritem3, 3, aq.pageSize()));
    REQUIRE(aq.isItemPage(ritem4, 4, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_CommitAt6At2At8_when_RetrieveCommitAt0At4Retrieve_then_Order68240)
{
    aq.advance(4);

    AQWriterItem witem4;
    AQWriterItem witem6;
    AQWriterItem witem8;
    AQWriterItem witem0;
    AQWriterItem witem2;
    REQUIRE(aq.writer.claim(witem4, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem6, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem8, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem2, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem6));
    REQUIRE(aq.writer.commit(witem2));
    REQUIRE(aq.writer.commit(witem8));

    AQItem ritem;
    AQItem ritem4;
    AQItem ritem6;
    AQItem ritem8;
    AQItem ritem0;
    AQItem ritem2;
    REQUIRE(aq.reader.retrieve(ritem6));
    REQUIRE(aq.reader.retrieve(ritem8));
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(!aq.reader.retrieve(ritem));

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.writer.commit(witem4));

    REQUIRE(aq.reader.retrieve(ritem4));
    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(!aq.reader.retrieve(ritem));

    REQUIRE(aq.isItemPage(ritem4, 4, aq.pageSize() * 2));
    REQUIRE(aq.isItemPage(ritem6, 6, aq.pageSize() * 2));
    REQUIRE(aq.isItemPage(ritem8, 8, aq.pageSize() * 2));
    REQUIRE(aq.isItemPage(ritem0, 0, aq.pageSize() * 2));
    REQUIRE(aq.isItemPage(ritem2, 2, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_LinkIdDisabled_when_Retrieve_then_LinkIdInvalid)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    witem.setLinkIdentifier(0x01020304);
    REQUIRE(aq.writer.commit(witem));

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(ritem.linkIdentifier() == AQItem::QUEUE_IDENTIFIER_INVALID);
}

//------------------------------------------------------------------------------
AQTEST(given_Crc32Disabled_when_Retrieve_then_ChecksumValid)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_UnclaimedAtHeadLessThan25PercentFreeSpace_when_BeforeCommitTimeout_then_ItemRemains)
{
    // Cause an unclaimed entry at the head to appear.
    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem;
    try
    {
        aq.writer.claim(witem, aq.pageSize());
        REQUIRE(false);
    }
    catch (...)
    {
    }

    // Fill the queue to less than 25% free.
    aq.enqueue(7);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));

    // Make sure it was consumed.
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_UnclaimedAtHeadMoreThan25PercentFreeSpace_when_CommitTimeout_then_ItemRemains)
{
    // Cause an unclaimed entry at the head to appear.
    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem;
    try
    {
        aq.writer.claim(witem, aq.pageSize());
        REQUIRE(false);
    }
    catch (...)
    {
    }

    // Fill the queue to more than 25% free.
    aq.enqueue(6);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem2));

    // Make sure it was not consumed.
    REQUIRE(aq.reader.availableSize() == 3 * aq.pageSize());
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_UnclaimedAtHeadLessThan25PercentFreeSpace_when_CommitTimeout_then_ItemFree)
{
    // Make sure the released space is all in a block.
    aq.advance(3);

    // Cause an unclaimed entry at the head to appear.
    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem;
    try
    {
        aq.writer.claim(witem, aq.pageSize());
        REQUIRE(false);
    }
    catch (...)
    {
    }

    // Fill the queue to less than 25% free.
    aq.enqueue(7);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem2));

    // Make sure it was consumed.
    REQUIRE(aq.reader.availableSize() == 3 * aq.pageSize());
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAtHeadLessThan25PercentFreeSpace_when_BeforeCommitTimeout_then_IncompleteRemains)
{
    // Cause an incomplete entry at the head to appear.
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    // Fill the queue to less than 25% free.
    aq.enqueue(7);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.areDifferentAllocatedItems(witem, ritem1));
    REQUIRE(aq.areDifferentAllocatedItems(witem, ritem2));

    // Make sure it was consumed.
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAtHeadMoreThan25PercentFreeSpace_when_CommitTimeout_then_IncompleteRemains)
{
    // Cause an incomplete entry at the head to appear.
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    // Fill the queue to more than 25% free.
    aq.enqueue(6);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.areDifferentAllocatedItems(witem, ritem1));
    REQUIRE(aq.areDifferentAllocatedItems(witem, ritem2));

    // Make sure it was not consumed.
    REQUIRE(aq.reader.availableSize() == 3 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAtHeadLessThan25PercentFreeSpace_when_CommitTimeout_then_IncompleteReturned)
{
    // Make sure the released space is all in a block.
    aq.advance(3);

    // Cause an incomplete entry at the head to appear.
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    // Fill the queue to less than 25% free.
    aq.enqueue(7);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.areDifferentAllocatedItems(witem, ritem1));
    REQUIRE(aq.areIdenticalAllocatedItems(witem, ritem2));

    // Make sure it was not discarded.
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAfterHeadLessThan25PercentFreeSpace_when_BeforeCommitTimeout_then_IncompleteRemains)
{
    // Cause an incomplete entry after the head to appear.
    AQWriterItem witem1, witem2;
    aq.writer.claim(witem1, aq.pageSize());
    aq.writer.claim(witem2, aq.pageSize());
    AQWriterItem tmp(witem1);
    aq.writer.commit(tmp);

    // Fill the queue to less than 25% free.
    aq.enqueue(7);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2, ritem3;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.areIdenticalAllocatedItems(witem1, ritem1));

    REQUIRE(aq.reader.retrieve(ritem3));
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.areDifferentAllocatedItems(witem2, ritem2));

    // Make sure it was not discarded.
    REQUIRE(aq.reader.availableSize() == 1 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAfterHeadMoreThan25PercentFreeSpace_when_CommitTimeout_then_IncompleteRemains)
{
    // Cause an incomplete entry after the head to appear.
    AQWriterItem witem1, witem2;
    aq.writer.claim(witem1, aq.pageSize());
    aq.writer.claim(witem2, aq.pageSize());
    AQWriterItem tmp(witem1);
    aq.writer.commit(tmp);

    // Fill the queue to more than 25% free.
    aq.enqueue(5);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2, ritem3;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.areIdenticalAllocatedItems(witem1, ritem1));

    REQUIRE(aq.reader.retrieve(ritem3));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.areDifferentAllocatedItems(witem2, ritem2));

    // Make sure it was not discarded.
    REQUIRE(aq.reader.availableSize() == 3 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAfterHeadLessThan25PercentFreeSpace_when_CommitTimeout_then_IncompleteReturned)
{
    // Cause an incomplete entry after the head to appear.
    AQWriterItem witem1, witem2;
    aq.writer.claim(witem1, aq.pageSize());
    aq.writer.claim(witem2, aq.pageSize());
    AQWriterItem tmp(witem1);
    aq.writer.commit(tmp);

    // Fill the queue to less than 25% free.
    aq.enqueue(7);

    // Release the two items - the queue should not change.
    AQItem ritem1, ritem2, ritem3;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.areIdenticalAllocatedItems(witem1, ritem1));

    REQUIRE(aq.reader.retrieve(ritem3));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.areIdenticalAllocatedItems(witem2, ritem2));

    // Make sure it was not discarded.
    REQUIRE(aq.reader.availableSize() == 1 * aq.pageSize());
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_UnclaimedThenWaste_when_CommitTimeout_then_BothFreed)
{
    aq.advance(7);

    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem1;
    try
    {
        aq.writer.claim(witem1, aq.pageSize());
        REQUIRE(false);
    }
    catch (...)
    {
    }

    AQItem ritem1, ritem2;
    REQUIRE(!aq.reader.retrieve(ritem1));

    AQWriterItem witem2;
    aq.writer.claim(witem2, aq.pageSize() * 5);
    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 1);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(!aq.reader.retrieve(ritem2));
    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 5);
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_QueueFullWithUncommittedAtHead_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(4);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(9);

    vector<AQItem> ritems;
    ritems.resize(9);
    for (size_t i = 0; i < ritems.size(); ++i)
    {
        aq.reader.retrieve(ritems[i]);
    }

    for (size_t i = 0; i < ritems.size(); ++i)
    {
        REQUIRE(aq.isItemPage(ritems[i], (5 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullWithUncommittedInMiddle_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(4);
    AQWriterItem witem;

    for (int i = 0; i < 10; ++i)
    {
        AQWriterItem witem;
        REQUIRE(aq.writer.claim(witem, aq.pageSize()));
        if (i != 4)
        {
            REQUIRE(aq.writer.commit(witem));
        }
    }

    vector<AQItem> ritems;
    ritems.resize(9);
    for (size_t i = 0; i < ritems.size(); ++i)
    {
        aq.reader.retrieve(ritems[i]);
    }

    size_t j = 4;
    for (size_t i = 0; i < ritems.size(); ++i)
    {
        REQUIRE(aq.isItemPage(ritems[i], j % aq.pageCount(), aq.pageSize()));
        if (i == 3)
        {
            j += 2;
        }
        else
        {
            j++;
        }
    }
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullMultiPageItemsUncommittedHead_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(3);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 2);
    aq.enqueue(1, aq.pageSize() * 1);
    aq.enqueue(1, aq.pageSize() * 3);
    aq.enqueue(1, aq.pageSize() * 1);
    aq.enqueue(1, aq.pageSize() * 2);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 2));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 3));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 2));
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullMultiPageItemsUncommittedInMiddle_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(3);
    AQWriterItem witem;
    aq.enqueue(1, aq.pageSize() * 1);
    aq.enqueue(1, aq.pageSize() * 2);
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    aq.enqueue(1, aq.pageSize() * 1);
    aq.enqueue(1, aq.pageSize() * 2);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 2));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 2));
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullWithWasteAndUncommittedAtHead_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(6);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 2);
    aq.enqueue(1, aq.pageSize() * 3);
    aq.enqueue(2, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 2));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 3));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullWithUncommittedBeforeWaste_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(6);
    AQWriterItem witem;
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    aq.enqueue(1, aq.pageSize() * 3);
    aq.enqueue(2, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 3));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullWithUncommittedAfterWaste_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(6);
    AQWriterItem witem;
    aq.enqueue(1, aq.pageSize() * 1);
    aq.enqueue(1, aq.pageSize() * 2);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    aq.enqueue(2, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 2));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFullWithUncommittedBeforeAndAfterWaste_when_MultipleRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(6);
    AQWriterItem witem;
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    aq.enqueue(1, aq.pageSize() * 2);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 2));
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentUncommitted_when_MultiRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(5);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 2, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentUncommittedAtLargestPage_when_MultiRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(8);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 9, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 2, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 5, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentMultiPageUncommitted_when_MultiRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(5);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    aq.enqueue(1, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentMultiPageUncommittedAroundWaste_when_MultiRetrieve_then_CommittedItemsRetrieved)
{
    aq.advance(6);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    aq.enqueue(2, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_ClaimHeadRefButCtrlQUnchangedFromPreviousRelease_when_RetrieveFinishCommitAndRetrieve_then_CommittedItemRetrieved)
{
    // Index[0] contains LEN=1, Index[1] contains LEN=3
    aq.advance(1);
    aq.advance(1, aq.pageSize() * 3);
    aq.advance(5);

    // Missing item at index '9', valid item at '10'.
    AQWriterItem witem9, witem0;
    REQUIRE(aq.writer.claim(witem9, aq.pageSize() * 1));
    aq.enqueue(1);

    // Pick out our valid item.
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    REQUIRE(!aq.reader.retrieve(ritem));

    // Claim an item at index [0] and [1], but run a retrieve() before the 
    // control is written.
    RetrieveAction retrieve(aq.reader);
    retrieve.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 3));
    REQUIRE(!retrieve.result());
    REQUIRE(!aq.reader.retrieve(ritem));

    // Commit and retrieve the items.
    aq.writer.commit(witem0);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 3));
    aq.writer.commit(witem9);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 9, aq.pageSize() * 1));
    REQUIRE(!aq.reader.retrieve(ritem));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_ClaimHeadRefButCtrlQUnchangedFromPreviousWaste_when_RetrieveFinishCommitAndRetrieve_then_CommittedItemRetrieved)
{
    // Index[7] contains LEN=1, Index[8] contains LEN=3 WASTE
    aq.advance(8);
    aq.advance(1, aq.pageSize() * 4);
    aq.advance(1);

    // Missing item at index '5', valid item at '6'.
    AQWriterItem witem5, witem7;
    REQUIRE(aq.writer.claim(witem5, aq.pageSize() * 1));
    aq.enqueue(1);

    // Pick out our valid item.
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(!aq.reader.retrieve(ritem));

    // Claim an item at index [7] [8] [9], but run a retrieve() before the 
    // control is written.
    RetrieveAction retrieve(aq.reader);
    retrieve.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    REQUIRE(aq.writer.claim(witem7, aq.pageSize() * 3));
    REQUIRE(!retrieve.result());
    REQUIRE(!aq.reader.retrieve(ritem));
    
    // Enqueue two more items and read them out.
    aq.enqueue(2);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 1));
    REQUIRE(!aq.reader.retrieve(ritem));

    // Commit and retrieve the items.
    aq.writer.commit(witem7);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 3));
    aq.writer.commit(witem5);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 5, aq.pageSize() * 1));
    REQUIRE(!aq.reader.retrieve(ritem));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_RetrieveAndClaimInterruptClaimBeforeCtrlqWrite_when_NextClaimInterruptedByRetreive_then_RetrieveNoRecord)
{

    aq.advance(11);
    aq.enqueue(1);
    AQItem ritem0, ritem1;
    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(aq.isItemPage(ritem0, 0, aq.pageSize() * 1));

    EnqueueAction enq(aq.writer, 1, aq.pageSize() * 3);
    enq.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    RetrieveAction retrieve(aq.reader);
    enq.runAfter(retrieve);

    AQWriterItem witem1;
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 6));
    REQUIRE(retrieve.result());
    REQUIRE(aq.isItemPage(retrieve.item(), 7, aq.pageSize() * 3));
    REQUIRE(aq.writer.commit(witem1));

    aq.reader.release(ritem0);
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.isItemPage(ritem1, 1, aq.pageSize() * 6));
    aq.reader.release(ritem1);
    aq.reader.release(retrieve.item());


    RetrieveAction retrieve2(aq.reader);
    retrieve2.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem0;
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 8));

    REQUIRE(!retrieve2.result());
    REQUIRE(aq.writer.commit(witem0));

    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(aq.isItemPage(ritem0, 0, aq.pageSize() * 8));
    aq.reader.release(ritem0);

    // Make sure we can fill the queue.
    aq.enqueue(10);
}
#endif




//=============================== End of File ==================================
