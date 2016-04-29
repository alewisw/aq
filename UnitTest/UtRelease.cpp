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
TEST_SUITE(UtRelease);

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItem_when_ReleaseMemNull_then_Exception)
{
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, NULL);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItem_when_ReleaseMem1ByteLarger_then_Exception)
{
    aq.advance(1);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &ritem[1]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItem_when_ReleaseMem1ByteSmaller_then_Exception)
{
    aq.advance(1);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &(&ritem[0])[-1]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItem_when_ReleaseMem1PageLarger_then_Exception)
{
    aq.advance(1);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &ritem[aq.pageSize()]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItem_when_ReleaseMem1PageSmaller_then_Exception)
{
    aq.advance(1);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &ritem[-(int)aq.pageSize()]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItemWithLowestMemoryAddress_when_ReleaseMem1ByteSmaller_then_Exception)
{
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &(&ritem[0])[-1]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItemWithLowestMemoryAddress_when_ReleaseMem1PageSmaller_then_Exception)
{
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &ritem[-(int)aq.pageSize()]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItemWithHighestMemoryAddress_when_ReleaseMem1ByteLarger_then_Exception)
{
    aq.advance(9);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &ritem[1]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_RetrieveItemWithHighestMemoryAddress_when_ReleaseMem1PageLarger_then_Exception)
{
    aq.advance(9);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &ritem[aq.pageSize()]);

    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ItemReleased_when_ReleaseAgain_then_Exception)
{
    aq.advance(9);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    AQItem ritem2 = ritem;
    aq.reader.release(ritem);

    REQUIRE_EXCEPTION(aq.reader.release(ritem2), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ItemRetrieved_when_ReleaseAtSameIndexButEarlierSeq_then_Exception)
{
    aq.advance(1);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    AQItem ritemCopy = ritem;
    REQUIRE(aq.isItemPage(ritemCopy, 1, aq.pageSize()));
    aq.reader.release(ritem);

    aq.advance(aq.pageCount() - 1);
    aq.enqueue(1);
    AQItem ritem2;
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.isItemPage(ritem2, 1, aq.pageSize()));

    REQUIRE_EXCEPTION(aq.reader.release(ritemCopy), invalid_argument);
    aq.reader.release(ritem2);
}

//------------------------------------------------------------------------------
AQTEST(given_ItemRetrieved_when_ReleaseBadMemory_then_ItemCleared)
{
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.mutateItemMem(ritem, &(&ritem[0])[-1]);
    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
    REQUIRE(!ritem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_ItemReleased_when_ItemReleaseAgain_then_ItemCleared)
{
    aq.advance(9);
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    AQItem ritem2 = ritem;
    aq.reader.release(ritem);
    REQUIRE_EXCEPTION(aq.reader.release(ritem2), invalid_argument);

    REQUIRE(!ritem.isAllocated());
    REQUIRE(!ritem2.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_ItemRetrieve_when_Released_then_ItemCleared)
{
    aq.enqueue(1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.reader.release(ritem);
    REQUIRE(!ritem.isAllocated());
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
    }
    catch (...)
    {
    }

    // Put two items in the queue and retreive them.
    AQItem ritem1, ritem2;
    aq.enqueue(2);
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));

    // Fill the queue to less than 25% free.
    aq.enqueue(5);
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());

    // Release the two items - the queue should not change, availableSize should be 
    aq.reader.release(ritem1);
    aq.reader.release(ritem2);

    // Make sure it was not consumed - note we do consume exactly one for the 
    // release case.
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_UnclaimedAtHeadMoreThan25PercentFreeSpact_when_CommitTimeout_then_ItemRemains)
{
    // Cause an unclaimed entry at the head to appear.
    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem;
    try
    {
        aq.writer.claim(witem, aq.pageSize());
    }
    catch (...)
    {
    }

    // Put two items in the queue and retreive them.
    AQItem ritem1, ritem2;
    aq.enqueue(2);
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));

    // Fill the queue to more than 25% free.
    aq.enqueue(4);
    REQUIRE(aq.reader.availableSize() == 3 * aq.pageSize());

    // Release the two items - the queue should not change.
    aq.reader.release(ritem1);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.reader.release(ritem2);

    // Make sure it was not consumed - note we do consume exactly one for the 
    // release case.
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
    }
    catch (...)
    {
    }

    // Put two items in the queue and retreive them.
    AQItem ritem1, ritem2;
    aq.enqueue(2);
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));

    // Fill the queue to less than 25% free.
    aq.enqueue(5);
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());

    // Release the two items - the queue should change on second release.
    aq.reader.release(ritem1);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.reader.release(ritem2);

    // Make sure it was consumed.
    REQUIRE(aq.reader.availableSize() == 5 * aq.pageSize());
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_IncompleteAtHeadLessThan25PercentFreeSpace_when_CommitTimeout_then_ItemRemains)
{
    // Make sure the released space is all in a block.
    aq.advance(3);

    // Cause an incomplete entry at the head to appear.
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    // Put two items in the queue and retreive them.
    AQItem ritem1, ritem2;
    aq.enqueue(2);
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));

    // Fill the queue to less than 25% free.
    aq.enqueue(5);
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());

    // Release the two items - the queue size must not change.
    aq.reader.release(ritem1);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.reader.release(ritem2);
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteRetrievedAtHead_when_Released_then_ItemFree)
{
    // Make sure the released space is all in a block.
    aq.advance(3);

    // Cause an incomplete entry at the head to appear.
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    AQItem ritem;
    REQUIRE(!aq.reader.retrieve(ritem));

    // Fill the queue so that there is no space available.
    aq.enqueue(9);
    REQUIRE(aq.reader.availableSize() == 0 * aq.pageSize());

    // Try to retrieve a term.
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.reader.availableSize() == 0 * aq.pageSize());
    aq.reader.release(ritem);
    REQUIRE(aq.reader.availableSize() == 1 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteReleasedAfterHead_when_HeadReleased_then_BothItemsFree)
{
    // Make sure the released space is all in a block.
    aq.advance(3);

    // Cause an incomplete entry at the head to appear.
    AQWriterItem witem0, witem1;
    aq.writer.claim(witem0, aq.pageSize());
    aq.writer.commit(witem0);
    aq.writer.claim(witem1, aq.pageSize());
    AQItem ritem0, ritem1;
    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(!aq.reader.retrieve(ritem1));

    // Fill the queue so that there is no space available.
    aq.enqueue(8);
    REQUIRE(aq.reader.availableSize() == 0 * aq.pageSize());

    // Try to retrieve a term.
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(!ritem1.isCommitted());
    aq.reader.release(ritem1);
    REQUIRE(aq.reader.availableSize() == 0 * aq.pageSize());
    aq.reader.release(ritem0);
    REQUIRE(aq.reader.availableSize() == 2 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFull_when_ReleaseAllItems_then_QueueEmpty)
{
    aq.advance(4);
    aq.enqueue(10);

    AQItem ritem;
    for (size_t i = 0; i < 10; ++i)
    {
        aq.reader.retrieve(ritem);
        REQUIRE(aq.isItemPage(ritem, (4 + i) % aq.pageCount(), aq.pageSize()));
        aq.reader.release(ritem);
    }

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_UncommittedInMidQueueAndAllItemsReleased_when_ItemCommittedAndReleased_then_QueueEmpty)
{
    aq.advance(4);
    aq.enqueue(4);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 1));
    aq.enqueue(5);

    AQItem ritem;
    size_t j = 4;
    for (size_t i = 0; i < 9; ++i)
    {
        REQUIRE(aq.reader.retrieve(ritem));
        REQUIRE(aq.isItemPage(ritem, j % aq.pageCount(), aq.pageSize()));
        if (i == 3)
        {
            j += 2;
        }
        else
        {
            j++;
        }
        aq.reader.release(ritem);
    }

    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 8, aq.pageSize()));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_MultipleSpacedUncommitted_when_CommittedAndReleased_then_QueueEmpty)
{
    aq.advance(3);
    AQWriterItem witem1;
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 2);
    aq.enqueue(1, aq.pageSize() * 1);
    AQWriterItem witem2;
    REQUIRE(aq.writer.claim(witem2, aq.pageSize() * 3));
    aq.enqueue(1, aq.pageSize() * 1);
    aq.enqueue(1, aq.pageSize() * 2);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 2));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 2));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem2));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 3));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    REQUIRE(!aq.writer.claim(witem1, aq.pageSize()));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_SpacedUncommittedAndWaste_when_CommittedAndReleased_then_QueueEmpty)
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
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 3));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 4, aq.pageSize() * 1));
    aq.reader.release(ritem);


    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    REQUIRE(!aq.writer.claim(witem, aq.pageSize()));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentUncommitted_when_CommitReleaseInOrder_then_QueueEmpty)
{
    aq.advance(5);
    AQWriterItem witem5, witem7, witem8, witem9, witem0, witem1;
    REQUIRE(aq.writer.claim(witem5, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem7, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem8, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem9, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 1));
    aq.enqueue(2, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 2, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem7));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem8));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 8, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem9));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 9, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 1, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem5));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 5, aq.pageSize() * 1));
    REQUIRE(!aq.writer.claim(witem0, aq.pageSize()));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentUncommitted_when_CommitReleaseInReverseOrder_then_QueueEmpty)
{
    aq.advance(5);
    AQWriterItem witem5, witem7, witem8, witem9, witem0, witem1;
    REQUIRE(aq.writer.claim(witem5, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem7, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem8, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem9, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 1));
    aq.enqueue(2, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 2, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 1, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem9));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 9, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem8));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 8, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem7));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem5));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 5, aq.pageSize() * 1));
    REQUIRE(!aq.writer.claim(witem0, aq.pageSize()));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentMultiPageUncommitted_when_CommitReleaseInOrder_then_QueueEmpty)
{
    aq.advance(5);
    AQWriterItem witem5, witem7, witem9, witem0, witem1;
    REQUIRE(aq.writer.claim(witem5, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem7, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem9, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 2));
    aq.enqueue(1, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem7));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 2));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem9));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 9, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 1, aq.pageSize() * 2));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem5));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 5, aq.pageSize() * 1));
    REQUIRE(!aq.writer.claim(witem0, aq.pageSize()));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_MultiAdjacentMultiPageUncommitted_when_CommitReleaseInReverseOrder_then_QueueEmpty)
{
    aq.advance(5);
    AQWriterItem witem5, witem7, witem9, witem0, witem1;
    REQUIRE(aq.writer.claim(witem5, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem7, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem9, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 2));
    aq.enqueue(1, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 10, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 1, aq.pageSize() * 2));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem9));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 9, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem7));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 2));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem5));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 5, aq.pageSize() * 1));
    REQUIRE(!aq.writer.claim(witem0, aq.pageSize()));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_ReleasedMiddle_when_ReleaseStartAndEnd_then_QueueEmpty)
{
    // Now rep
    AQWriterItem witem0, witem1;
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 1));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 1));
    aq.enqueue(3);
    AQItem ritem0, ritem1, ritem2, ritem3, ritem4;
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.isItemPage(ritem2, 2, aq.pageSize() * 1));
    aq.reader.release(ritem2);
    REQUIRE(aq.reader.retrieve(ritem3));
    REQUIRE(aq.isItemPage(ritem3, 3, aq.pageSize() * 1));
    aq.reader.release(ritem3);

    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.isItemPage(ritem1, 1, aq.pageSize() * 1));
    aq.reader.release(ritem1);

    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.reader.retrieve(ritem0));
    REQUIRE(aq.isItemPage(ritem0, 0, aq.pageSize() * 1));
    aq.reader.release(ritem0);

    REQUIRE(aq.reader.retrieve(ritem4));
    REQUIRE(aq.isItemPage(ritem4, 4, aq.pageSize() * 1));
    aq.reader.release(ritem4);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_Waste_when_ReleaseItemBefore_then_WasteFree)
{
    aq.advance(7);
    aq.enqueue(1);
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize() * 5);
    aq.writer.commit(witem);

    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 1);
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.reader.release(ritem);
    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 5);
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_Waste_when_DiscardUnclaimedBefore_then_WasteFree)
{
    aq.advance(6);
    aq.enqueue(1);

    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem1;
    try
    {
        aq.writer.claim(witem1, aq.pageSize());
    }
    catch (...)
    {
    }
    AQWriterItem witem2;
    aq.writer.claim(witem2, aq.pageSize() * 4);
    aq.writer.commit(witem2);

    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 1);
    AQItem ritem1, ritem2;
    REQUIRE(aq.reader.retrieve(ritem1));
    REQUIRE(aq.reader.retrieve(ritem2));
    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 1);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.reader.release(ritem1);
    REQUIRE(aq.reader.availableSize() == aq.pageSize() * 6);
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_ReleasedBeforeWaste_when_CommitReleaseTail_then_QueueAtStart)
{
    aq.advance(6);
    AQWriterItem witem6, witem8;
    REQUIRE(aq.writer.claim(witem6, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem8, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 3);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem8));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 8, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem6));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.availableSize() == (aq.pageCount() - 3 - 1) * aq.pageSize());

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 3));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
AQTEST(given_ReleasedAfterWaste_when_CommitReleaseTail_then_QueueAtStart)
{
    aq.advance(6);
    AQWriterItem witem6, witem8;
    REQUIRE(aq.writer.claim(witem6, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 1);
    REQUIRE(aq.writer.claim(witem8, aq.pageSize() * 1));
    aq.enqueue(1, aq.pageSize() * 3);
    aq.enqueue(1, aq.pageSize() * 1);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 3));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem8));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 8, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.writer.commit(witem6));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 6, aq.pageSize() * 1));
    aq.reader.release(ritem);

    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 3, aq.pageSize() * 1));
    aq.reader.release(ritem);

    aq.enqueue(10);
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_ClaimButCtrlQUnchangedFromPreviousRelease_when_ReleasePrevious_then_ClaimedRecordRemains)
{
    // Index[0] contains LEN=1, Index[1] contains LEN=3
    aq.advance(1);
    aq.advance(1, aq.pageSize() * 3);
    aq.advance(7);

    // Valid item at '0'.
    aq.enqueue(1);

    // Pick out our valid item.
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 0, aq.pageSize() * 1));

    // Claim an item at index [1] but run release() before the control is written
    // so it sees the previous '3' entry in the control field.
    ReleaseAction release(aq.reader, ritem);
    release.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem1;
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 1));

    // Commit and retrieve the items.
    aq.writer.commit(witem1);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 1, aq.pageSize() * 1));
    aq.reader.release(ritem);
    REQUIRE(!aq.reader.retrieve(ritem));

    // Make sure the queue is empty.
    aq.enqueue(10);
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_ClaimButCtrlQUnchangedFromPreviousWaste_when_ReleasePrevious_then_ClaimedRecordRemains)
{
    // Index[7] contains LEN=1, Index[8] contains LEN=3 from Waste
    aq.advance(8);
    aq.advance(1, aq.pageSize() * 4);
    aq.advance(3);

    // Valid item at '7'.
    aq.enqueue(1);

    // Pick out our valid item.
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 7, aq.pageSize() * 1));

    // Claim an item at index [8] but run release() before the control is written
    // so it sees the previous LEN='3' entry in the control field.
    ReleaseAction release(aq.reader, ritem);
    release.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    AQWriterItem witem1;
    REQUIRE(aq.writer.claim(witem1, aq.pageSize() * 1));

    // Commit and retrieve the items.
    aq.writer.commit(witem1);
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemPage(ritem, 8, aq.pageSize() * 1));
    aq.reader.release(ritem);
    REQUIRE(!aq.reader.retrieve(ritem));

    // Make sure the queue is empty.
    aq.enqueue(10);
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_ItemInQueue_when_Retrieve_FreeCounterUnchanged)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    aq.writer.commit(witem);

    uint32_t c1 = aq.writer.freeCounter();

    AQItem ritem;
    aq.reader.retrieve(ritem);

    uint32_t c2 = aq.writer.freeCounter();
    REQUIRE(c1 == c2);
}

//------------------------------------------------------------------------------
AQTEST(given_ItemReleased_when_NotFreed_FreeCounterUnchanged)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    aq.writer.commit(witem);
    aq.writer.claim(witem, aq.pageSize());
    aq.writer.commit(witem);

    uint32_t c1 = aq.writer.freeCounter();

    AQItem ritem1, ritem2;
    aq.reader.retrieve(ritem1);
    aq.reader.retrieve(ritem2);
    aq.reader.release(ritem2);

    uint32_t c2 = aq.writer.freeCounter();
    REQUIRE(c1 == c2);
}

//------------------------------------------------------------------------------
AQTEST(given_FreeCounterLow_when_ItemFreed_FreeCounterIncrements)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    aq.writer.commit(witem);

    (uint32_t&)aq.writer.freeCounter() = 0;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    aq.reader.release(ritem);

    uint32_t c1 = aq.writer.freeCounter();
    REQUIRE(c1 == 1);
}

//------------------------------------------------------------------------------
AQTEST(given_FreeCounterMid_when_ItemFreed_FreeCounterIncrements)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    aq.writer.commit(witem);

    (uint32_t&)aq.writer.freeCounter() = 0x7FFFFFFF;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    aq.reader.release(ritem);

    uint32_t c1 = aq.writer.freeCounter();
    REQUIRE(c1 == 0x80000000);
}

//------------------------------------------------------------------------------
AQTEST(given_FreeCounterHigh_when_ItemFreed_FreeCounterWraps)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    aq.writer.commit(witem);

    (uint32_t&)aq.writer.freeCounter() = 0xFFFFFFFF;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    aq.reader.release(ritem);

    uint32_t c1 = aq.writer.freeCounter();
    REQUIRE(c1 == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_IncompleteRecordRetrieved_when_CommitRelease_ReleaseCorrectly)
{
    AQWriterItem witem;
    CHECK(aq.writer.claim(witem, aq.pageSize()));
    CHECK(aq.isItemPage(witem, 0, aq.pageSize()));

    // Start the timer.
    AQItem ritem;
    CHECK(!aq.reader.retrieve(ritem));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.enqueue(8);

    CHECK(aq.reader.retrieve(ritem));
    CHECK(!ritem.isCommitted());

    // Commit it now.
    CHECK(aq.writer.commit(witem));

    // Make sure release succeeds.
    aq.reader.release(ritem);
    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 9);
    REQUIRE(snap[0].isCommitted());
    REQUIRE(snap[0].isReleased());
    REQUIRE(aq.isItemPage(snap[0], 0, aq.pageSize()));
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
static void corruptCtrlqIndex0(AQ *queue, void *context)
{
    (void)queue;

    AQTest& aq = *((AQTest *)context);

    aq.ctrl->ctrlq[0] &= ~CtrlOverlay::CTRLQ_CLAIM_MASK;
}
AQTEST(given_IncompleteRecordRetrieved_when_CommitReleaseCorruptCtrlqBeforeSecondWriteCtrl_Exception)
{
    AQWriterItem witem;
    CHECK(aq.writer.claim(witem, aq.pageSize()));
    CHECK(aq.isItemPage(witem, 0, aq.pageSize()));

    // Start the timer.
    AQItem ritem;
    CHECK(!aq.reader.retrieve(ritem));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.enqueue(8);

    CHECK(aq.reader.retrieve(ritem));
    CHECK(!ritem.isCommitted());

    // Commit it now.
    CHECK(aq.writer.commit(witem));

    // Corrupt it during the second release update.
    aq.reader.testPointNotifier()->registerTestPoint(AQReader::ReleaseBeforeWriteSecondCtrl, 
        corruptCtrlqIndex0, NULL, &aq);
    REQUIRE_EXCEPTION(aq.reader.release(ritem), invalid_argument);
}
#endif




//=============================== End of File ==================================
