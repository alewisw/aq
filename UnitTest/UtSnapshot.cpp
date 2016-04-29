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

// The number of full queue usage events before sequence number rotation.
#define SEQ_ROTATE_COUNT                ((CtrlOverlay::CTRLQ_SEQ_MASK >> CtrlOverlay::REF_SEQ_SHIFT) + 1)




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
TEST_SUITE(UtSnapshot);

//------------------------------------------------------------------------------
TEST(given_QueueMemoryNull_when_Snapshot_then_AQUnformattedException)
{
    AQReader q(NULL, 10000);

    AQSnapshot snap;
    REQUIRE_EXCEPTION(snap.snap(q), AQUnformattedException);
    REQUIRE(snap.size() == 0);
}

//------------------------------------------------------------------------------
TEST(given_QueueMemoryTooSmall_when_Snapshot_then_AQUnformattedException)
{
    char mem[1000];
    AQReader q(mem, sizeof(CtrlOverlay) - 1);

    AQSnapshot snap;
    REQUIRE_EXCEPTION(snap.snap(q), AQUnformattedException);
    REQUIRE(snap.size() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Formatted_when_Snapshot_then_NoItems)
{
    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Snapshot_when_CopyConstruct_then_CopiedObjectIdentical)
{
    AQWriterItem witem;
    AQItem ritem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.reader.retrieve(ritem));

    AQSnapshot snapBase(aq.reader, aq.trace);
    AQSnapshot snap(snapBase);

    REQUIRE(snap.size() == 2);
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[0], 0, aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 1, aq.pageSize()));
}


//------------------------------------------------------------------------------
AQTEST(given_Snapshot_when_Assign_then_CopiedObjectIdentical)
{
    AQSnapshot snap(aq.writer, aq.trace);

    AQWriterItem witem;
    AQItem ritem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.reader.retrieve(ritem));

    AQSnapshot snapBase(aq.reader, aq.trace);
    snap = snapBase;

    REQUIRE(snap.size() == 2);
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[0], 0, aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 1, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_CommitAt0At1_when_Snapshot_then_2UnreleasedItems)
{
    AQWriterItem witem0;
    AQWriterItem witem1;
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.writer.commit(witem1));

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 2);
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[0], 0, aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 1, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_CommitAt0At1RetreiveAt0At1_when_Snapshot_then_2UnreleasedItems)
{
    AQWriterItem witem;
    AQItem ritem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.reader.retrieve(ritem));

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 2);
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[0], 0, aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 1, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_CommitAt0At1ReleaseAt0At1_when_Snapshot_then_2ReleasedItems)
{
    AQWriterItem witem;
    AQItem ritem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(aq.reader.retrieve(ritem));
    aq.reader.release(ritem);
    REQUIRE(aq.reader.retrieve(ritem));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 2);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 0, aq.pageSize()));
    REQUIRE(aq.isReleasedCommittedItemPage(snap[1], 1, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt5TailAt3_when_Snapshot_then_9Released2UnreleasedItems)
{
    aq.advance(14);
    aq.enqueue(2);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 9; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (5 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 2; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 9], (3 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt3TailAt5_when_Snapshot_then_2Released9UnreleasedItems)
{
    aq.advance(5);
    aq.enqueue(9);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 2; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (3 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 9; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 2], (5 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt10TailAt0_when_Snapshot_then_1Released10UnreleasedItems)
{
    aq.advance(11);
    aq.enqueue(10);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 1; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (10 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 10; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 1], (0 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt0TailAt10_when_Snapshot_then_10Released1UnreleasedItems)
{
    aq.advance(11 + 10);
    aq.enqueue(1);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 10; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (0 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 1; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 10], (10 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt0TailAt0_when_Snapshot_then_11UnreleasedItems)
{
    aq.advance(11 + 11);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 11; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (0 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt5TailAt5_when_Snapshot_then_11UnreleasedItems)
{
    aq.advance(11 + 5);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 11; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (5 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HeadAt10TailAt10_when_Snapshot_then_11UnreleasedItems)
{
    aq.advance(11 + 10);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 11);
    for (int i = 0; i < 11; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (10 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_Size4Size3LoopSize2Size2_when_Snapshot_then_4Items)
{
    aq.advance(7);
    aq.advance(1, 4 * aq.pageSize());
    aq.enqueue(1, 3 * aq.pageSize());
    aq.enqueue(2, 2 * aq.pageSize());

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 4);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 7, 4 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 0, 3 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[2], 3, 2 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[3], 5, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_Size4Size1Overwrite2LoopSize2Size2_when_Snapshot_then_4Items)
{
    aq.advance(1, 3 * aq.pageSize());
    aq.advance(1, 4 * aq.pageSize());
    aq.advance(1, 4 * aq.pageSize());
    aq.enqueue(1, 1 * aq.pageSize());
    aq.enqueue(2, 2 * aq.pageSize());

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 4);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 7, 4 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 0, 1 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[2], 1, 2 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[3], 3, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_Advance11Size4Size1Overwrite2LoopSize2Size2_when_Snapshot_then_4Items)
{
    aq.advance(11);
    given_Size4Size1Overwrite2LoopSize2Size2_when_Snapshot_then_4Items();
}

//------------------------------------------------------------------------------
AQTEST(given_Advance7Waste4Size5_when_Snapshot_then_3Items)
{
    aq.advance(7);
    aq.enqueue(1, 5 * aq.pageSize());

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 3);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 5, 1 * aq.pageSize()));
    REQUIRE(aq.isReleasedCommittedItemPage(snap[1], 6, 1 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[2], 0, 5 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_Advance18Waste4Size5_when_Snapshot_then_3Items)
{
    aq.advance(11);
    given_Advance7Waste4Size5_when_Snapshot_then_3Items();
}

//------------------------------------------------------------------------------
AQTEST(given_Advance7Waste4Size5Size2Size1_when_Snapshot_then_3Items)
{
    aq.advance(7);
    aq.advance(1, 5 * aq.pageSize());
    aq.enqueue(1, 2 * aq.pageSize());
    aq.enqueue(1, 1 * aq.pageSize());

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 3);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 0, 5 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[1], 5, 2 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[2], 7, 1 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_Advance18Waste4Size5Size2Size1_when_Snapshot_then_3Items)
{
    aq.advance(11);
    given_Advance7Waste4Size5Size2Size1_when_Snapshot_then_3Items();
}

//------------------------------------------------------------------------------
AQTEST(given_UnreleasedIncomplete_when_Snapshot_then_ItemUnreleasedIncomplete)
{
    AQWriterItem witem;

    aq.enqueue(1);
    CHECK(aq.writer.claim(witem, aq.pageSize() * 2));

    AQItem ritem0;
    AQItem ritem1;

    CHECK(aq.reader.retrieve(ritem0));
    CHECK(aq.isItemPage(ritem0, 0, aq.pageSize()));
    CHECK(!aq.reader.retrieve(ritem1));
        
    aq.enqueue(aq.pageCount() - 4);

    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);

    CHECK(aq.reader.retrieve(ritem1));
    CHECK(aq.isItemPage(ritem1, 1, 2 * aq.pageSize()));

    AQSnapshot snap(aq.writer, aq.trace);
    REQUIRE(aq.isUnreleasedUncommittedItemPage(snap[1], 1, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_ReleasedIncomplete_when_Snapshot_then_ItemReleasedIncomplete)
{
    AQWriterItem witem;

    aq.enqueue(1);
    CHECK(aq.writer.claim(witem, aq.pageSize() * 2));

    AQItem ritem0;
    AQItem ritem1;

    CHECK(aq.reader.retrieve(ritem0));
    CHECK(aq.isItemPage(ritem0, 0, aq.pageSize()));
    CHECK(!aq.reader.retrieve(ritem1));

    aq.enqueue(aq.pageCount() - 4);

    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);

    CHECK(aq.reader.retrieve(ritem1));
    CHECK(aq.isItemPage(ritem1, 1, 2 * aq.pageSize()));
    aq.reader.release(ritem1);

    AQSnapshot snap(aq.writer, aq.trace);
    REQUIRE(aq.isReleasedUncommittedItemPage(snap[1], 1, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_DiscardedIncomplete_when_Snapshot_then_ItemReleasedIncomplete)
{
    AQWriterItem witem;

    aq.enqueue(1);
    CHECK(aq.writer.claim(witem, aq.pageSize() * 2));

    AQItem ritem0;
    AQItem ritem1;

    CHECK(aq.reader.retrieve(ritem0));
    CHECK(aq.isItemPage(ritem0, 0, aq.pageSize()));
    CHECK(!aq.reader.retrieve(ritem1));

    aq.enqueue(aq.pageCount() - 4);

    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);

    CHECK(aq.reader.retrieve(ritem1));
    CHECK(aq.isItemPage(ritem1, 1, 2 * aq.pageSize()));
    aq.reader.release(ritem1);
    aq.reader.release(ritem0);

    AQSnapshot snap(aq.writer, aq.trace);
    REQUIRE(aq.isReleasedUncommittedItemPage(snap[1], 1, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_WasteContainsSeq2Recs_when_RefSeqLoopsAndSnapshot_WasteNotReturnedAsItems)
{
    // Fills the queue with SEQ=2 CTRL-Q entries.
    aq.advance(aq.pageCount() * 3, 1 * aq.pageSize());
    
    // Force Waste at end of queue and rotate it to realign sequence numbers.
    aq.advance(2 * (SEQ_ROTATE_COUNT + 1), 4 * aq.pageSize());                 

    // Overwrite the first page of the waste to make the rest visible to the snapshot.
    aq.enqueue(1);                                     

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 3);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 0, 4 * aq.pageSize()));
    REQUIRE(aq.isReleasedCommittedItemPage(snap[1], 4, 4 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[2], 8, 1 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_FillSeq2Recs_when_LargeRecsAndRefSeqLoops_then_SnapshotDoesNotContainOldItems)
{
    // Fills the queue with SEQ=2 CTRL-Q entries.
    aq.advance(aq.pageCount() * 3, 1 * aq.pageSize());

    // Exactly fill the queue until the sequence numbers have rotated.
    for (uint32_t i = 0; i < SEQ_ROTATE_COUNT; ++i)
    {
        aq.advance(2, 4 * aq.pageSize());
        aq.advance(1, 3 * aq.pageSize());
    }

    // Overwrite the first page of the waste to make the rest visible to the snapshot.
    aq.enqueue(1);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap.size() == 3);
    REQUIRE(aq.isReleasedCommittedItemPage(snap[0], 4, 4 * aq.pageSize()));
    REQUIRE(aq.isReleasedCommittedItemPage(snap[1], 8, 3 * aq.pageSize()));
    REQUIRE(aq.isUnreleasedCommittedItemPage(snap[2], 0, 1 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST(given_HalfFullQueue_when_Claim1AfterSnapshotHeadRef_then_ClaimedItemNotReturned)
{
    AQWriterItem witem;

    aq.advance(14);
    aq.enqueue(6);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.writer.claim(witem, aq.pageSize());
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE(snap.size() == 10);
    for (int i = 0; i < 4; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (10 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 6; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 4], (3 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_HalfFullQueue_when_Claim2AfterSnapshotHeadRef_then_ClaimedItemNotReturned)
{
    AQWriterItem witem;

    aq.advance(14);
    aq.enqueue(6);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.writer.claim(witem, 2 * aq.pageSize());
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE(snap.size() == 9);
    for (int i = 0; i < 3; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (0 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 6; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 3], (3 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HalfFullQueue_when_SnapshotAfterHeadRefChange_then_ClaimedItemNotReturned)
{
    AQWriterItem witem;

    aq.advance(14);
    aq.enqueue(6);

    AQSnapshot snap(aq.trace);

    SnapshotAction action(aq.writer, snap, 1, 2, 3);
    action.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    aq.writer.claim(witem, aq.pageSize());

    *(uint32_t *)(&witem[0]) = 0xD00DC1C1;
    snap.snap4FinalHead();

    REQUIRE(snap.size() == 10);
    for (int i = 0; i < 4; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (10 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 6; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 4], (3 + i) % aq.pageCount(), aq.pageSize()));
    }
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_WholeQueueRotatedDuringSnapshot_when_FinalHeadIndexGreaterThanInitHeadIndex_then_ZeroItemsReturned)
{
    aq.advance(14);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.advance(10);
    aq.enqueue(3);
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE((snap.finalHeadRef() & CtrlOverlay::REF_INDEX_MASK) > (snap.initHeadRef() & CtrlOverlay::REF_INDEX_MASK));
    REQUIRE(snap.size() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WholeQueueRotatedDuringSnapshot_when_FinalHeadIndexLessThanInitHeadIndex_then_ZeroItemsReturned)
{
    aq.advance(7);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.advance(17);
    aq.enqueue(3);
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE((snap.finalHeadRef() & CtrlOverlay::REF_INDEX_MASK) < (snap.initHeadRef() & CtrlOverlay::REF_INDEX_MASK));
    REQUIRE(snap.size() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WholeQueueRotatedDuringSnapshot_when_FinalHeadIndexEqualsInitHeadIndex_then_ZeroItemsReturned)
{
    aq.advance(14);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.advance(5);
    aq.enqueue(6);
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE((snap.finalHeadRef() & CtrlOverlay::REF_INDEX_MASK) == (snap.initHeadRef() & CtrlOverlay::REF_INDEX_MASK));
    REQUIRE(snap.size() == 0);
}


//------------------------------------------------------------------------------
AQTEST(given_PartQueueRotatedDuringSnapshot_when_FinalHeadIndexGreaterThanInitHeadIndex_then_NewItemsNotReturned)
{
    aq.advance(11);
    aq.enqueue(1);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.enqueue(4);
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE((snap.finalHeadRef() & CtrlOverlay::REF_INDEX_MASK) > (snap.initHeadRef() & CtrlOverlay::REF_INDEX_MASK));
    REQUIRE(snap.size() == 7);
    for (int i = 0; i < 6; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (5 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 1; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 6], (0 + i) % aq.pageCount(), aq.pageSize()));
    }
}

//------------------------------------------------------------------------------
AQTEST(given_PartQueueRotatedDuringSnapshot_when_FinalHeadIndexLessThanInitHeadIndex_then_NewItemsNotReturned)
{
    aq.advance(19);
    aq.enqueue(1);

    AQSnapshot snap(aq.trace);
    snap.snap1InitialHead(aq.reader);
    aq.enqueue(4);
    snap.snap2InitialCtrlq();
    snap.snap3PageMemory();
    snap.snap4FinalHead();

    REQUIRE((snap.finalHeadRef() & CtrlOverlay::REF_INDEX_MASK) < (snap.initHeadRef() & CtrlOverlay::REF_INDEX_MASK));
    REQUIRE(snap.size() == 7);
    for (int i = 0; i < 6; ++i)
    {
        REQUIRE(aq.isReleasedCommittedItemPage(snap[i], (2 + i) % aq.pageCount(), aq.pageSize()));
    }
    for (int i = 0; i < 1; ++i)
    {
        REQUIRE(aq.isUnreleasedCommittedItemPage(snap[i + 6], (8 + i) % aq.pageCount(), aq.pageSize()));
    }
}




//=============================== End of File ==================================
