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
TEST_SUITE(UtCommit);

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitMemNull_then_Exception)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, NULL);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitMem1ByteLarger_then_Exception)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[1]);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitMem1ByteSmaller_then_Exception)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &(&witem[0])[-1]);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitMem1PageLarger_then_Exception)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[aq.pageSize()]);
    REQUIRE(!aq.writer.commit(witem));
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitMem1PageSmaller_then_Exception)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[-(int)aq.pageSize()]);
    REQUIRE(!aq.writer.commit(witem));
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItemWithLowestMemoryAddress_when_CommitMem1ByteSmaller_then_Exception)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &(&witem[0])[-1]);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItemWithLowestMemoryAddress_when_CommitMem1PageSmaller_then_Exception)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[-(int)aq.pageSize()]);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItemWithHighestMemoryAddress_when_Mem1ByteLarger_then_Exception)
{
    aq.advance(10);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[1]);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItemWithHighestMemoryAddress_when_Mem1PageLarger_then_Exception)
{
    aq.advance(10);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[aq.pageSize()]);

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
}


//------------------------------------------------------------------------------
AQTEST(given_ItemCommitted_when_CommittedAgain_then_Exception)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    AQWriterItem witemCopy = witem;
    REQUIRE(aq.writer.commit(witemCopy));
    REQUIRE(!aq.writer.commit(witem));
}

//------------------------------------------------------------------------------
AQTEST(given_ItemClaimed_when_CommitAtSameIndexButEarlierSeq_then_Exception)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    AQWriterItem witemCopy = witem;
    REQUIRE(aq.isItemPage(witemCopy, 1, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    aq.reader.release(ritem);

    aq.advance(aq.pageCount() - 1);
    REQUIRE(aq.writer.claim(witemCopy, aq.pageSize()));
    REQUIRE(aq.isItemPage(witemCopy, 1, aq.pageSize()));

    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);

    REQUIRE(aq.writer.commit(witemCopy));
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitBadMemory_then_ItemFieldsReset)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.mutateItemMem(witem, &witem[1]);
    REQUIRE_EXCEPTION(aq.writer.commit(witem), invalid_argument);
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_DoubleCommit_then_ItemFieldsReset)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    AQWriterItem witemCopy = witem;
    REQUIRE(aq.writer.commit(witemCopy));
    REQUIRE(!aq.writer.commit(witem));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimItem_when_CommitSucceeds_then_ItemFieldsReset)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimAt0_when_Commit_then_CommitSucceeds)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem));
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimAt9At10At0_when_Commit10Commit0Commit9_CommitsSucceeds)
{
    aq.advance(9);

    AQWriterItem witem9;
    AQWriterItem witem10;
    AQWriterItem witem0;
    REQUIRE(aq.writer.claim(witem9, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem10, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem0, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem10));
    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.writer.commit(witem9));
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimAt9At0At1_when_Commit0Commit1Commit9_CommitsSucceeds)
{
    aq.advance(9);

    AQWriterItem witem9;
    AQWriterItem witem1;
    AQWriterItem witem0;
    REQUIRE(aq.writer.claim(witem9, aq.pageSize()));
    REQUIRE(aq.writer.claim(witem0, aq.pageSize() * 2));
    REQUIRE(aq.writer.claim(witem1, aq.pageSize()));
    REQUIRE(aq.writer.commit(witem0));
    REQUIRE(aq.writer.commit(witem1));
    REQUIRE(aq.writer.commit(witem9));
}

//------------------------------------------------------------------------------
AQTEST(given_CommitCounterRead_ItemClaimed_CommitCounterUnchanged)
{
    uint32_t c1 = aq.reader.commitCounter();

    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    uint32_t c2 = aq.reader.commitCounter();

    REQUIRE(c1 == c2);
}

//------------------------------------------------------------------------------
AQTEST(given_ClaimedItemExpired_ItemCommitted_CommitCounterUnchanged)
{

    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    aq.enqueue(9);
    AQItem ritem;
    for (int i = 0; i < 9; ++i)
    {
        REQUIRE(aq.reader.retrieve(ritem));
        aq.reader.release(ritem);
    }

    uint32_t c1 = aq.reader.commitCounter();

    REQUIRE(!aq.reader.retrieve(ritem));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    REQUIRE(aq.reader.retrieve(ritem));
    aq.reader.release(ritem);
    REQUIRE(!aq.writer.commit(witem));

    uint32_t c2 = aq.reader.commitCounter();

    REQUIRE(c1 == c2);
}

//------------------------------------------------------------------------------
AQTEST(given_CommitCounterMin_ItemCommitted_then_CommitCounterIncrements)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    (uint32_t& )aq.reader.commitCounter() = 0;

    aq.writer.commit(witem);

    uint32_t c2 = aq.reader.commitCounter();

    REQUIRE(c2 == 1);
}

//------------------------------------------------------------------------------
AQTEST(given_CommitCounterMid_ItemCommitted_then_CommitCounterIncrements)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    (uint32_t&)aq.reader.commitCounter() = 0x7FFFFFFF;

    aq.writer.commit(witem);

    uint32_t c2 = aq.reader.commitCounter();

    REQUIRE(c2 == 0x80000000);
}

//------------------------------------------------------------------------------
AQTEST(given_CommitCounterMax_ItemCommitted_then_CommitCounterWraps)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());

    (uint32_t&)aq.reader.commitCounter() = 0xFFFFFFFF;

    aq.writer.commit(witem);

    uint32_t c2 = aq.reader.commitCounter();

    REQUIRE(c2 == 0);
}



//=============================== End of File ==================================
