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




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtExtendableWriter);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_SizeZero_when_Claim_then_Succeeds1Page, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    aq.writer.claim(item, 0);
    REQUIRE(item.size() == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_5PagesAvailable_when_Extend6Pages_then_ExtendFails, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, 5 * aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == 5 * aq.pageSize());
    REQUIRE(aq.appendData(item, 0, 5 * aq.pageSize()));
    REQUIRE(!aq.appendData(item, 0, 6 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_5PagesAvailable_when_Extend6Pages_then_ItemUnchanged, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, 5 * aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == 5 * aq.pageSize());
    REQUIRE(aq.appendData(item, 0, 5 * aq.pageSize()));
    AQWriterItem itemCmp(item);
    REQUIRE(!aq.appendData(item, 0, 6 * aq.pageSize()));
    
    REQUIRE(aq.areIdenticalAllocatedItems(item, itemCmp));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_5PagesAvailable_when_Extend6Pages_then_QueueUnchanged, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, 5 * aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == 5 * aq.pageSize());
    REQUIRE(aq.appendData(item, 0, 5 * aq.pageSize()));

    CtrlOverlay *cpy = (CtrlOverlay *)malloc(aq.ctrl->size);
    memcpy(cpy, aq.ctrl, aq.ctrl->size);

    AQWriterItem itemCmp(item);
    REQUIRE(!aq.appendData(item, 0, 6 * aq.pageSize()));

    REQUIRE(memcmp(cpy, aq.ctrl, aq.ctrl->size) == 0);
    free(cpy);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_2Bytes4PagesAvailable_when_Extend6Pages_then_ExtendFails, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, 5 * aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == 5 * aq.pageSize());
    REQUIRE(aq.appendData(item, 0, 5 * aq.pageSize() - 2));
    REQUIRE(!aq.appendData(item, 0, 6 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_2Bytes4PagesAvailable_when_Extend6Pages_then_ItemUnchanged, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, 5 * aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == 5 * aq.pageSize());
    REQUIRE(aq.appendData(item, 0, 5 * aq.pageSize() - 2));
    AQWriterItem itemCmp(item);
    REQUIRE(!aq.appendData(item, 0, 6 * aq.pageSize()));

    REQUIRE(aq.areIdenticalAllocatedItems(item, itemCmp));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_2Bytes4PagesAvailable_when_Extend6Pages_then_QueueUnchanged, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, 5 * aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == 5 * aq.pageSize());
    REQUIRE(aq.appendData(item, 0, 5 * aq.pageSize() - 2));

    CtrlOverlay *cpy = (CtrlOverlay *)malloc(aq.ctrl->size);
    memcpy(cpy, aq.ctrl, aq.ctrl->size);

    AQWriterItem itemCmp(item);
    REQUIRE(!aq.appendData(item, 0, 6 * aq.pageSize()));

    REQUIRE(memcmp(cpy, aq.ctrl, aq.ctrl->size) == 0);
    free(cpy);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_MultiItemCommit_when_OneCommitFails_then_CommitReportsFailure, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    CHECK(aq.writer.claim(item, 1 * aq.pageSize()));
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK(aq.appendData(item, 0, 1 * aq.pageSize()));
    }

    // Force the second commit to fail by manipuating the control queue entry.
    aq.ctrl->ctrlq[1] |= CtrlOverlay::CTRLQ_DISCARD_MASK;

    REQUIRE(!aq.writer.commit(item));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_MultiItemCommit_when_FirstCommitFails_then_RemainingItemsNotCommitted, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    CHECK(aq.writer.claim(item, 1 * aq.pageSize()));
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK(aq.appendData(item, 0, 1 * aq.pageSize()));
    }

    // Force the first commit to fail by manipuating the control queue entry.
    aq.ctrl->ctrlq[0] |= CtrlOverlay::CTRLQ_DISCARD_MASK;
    CHECK(!aq.writer.commit(item));
    aq.ctrl->ctrlq[0] &= ~CtrlOverlay::CTRLQ_DISCARD_MASK;

    // Now verify none of the items were committed.
    for (size_t i = 0; i < 3; ++i)
    {
        REQUIRE(!(aq.ctrl->ctrlq[i] & CtrlOverlay::CTRLQ_COMMIT_MASK));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_MultiItemCommit_when_SecondCommitFails_then_RemainingItemsNotCommitted, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    CHECK(aq.writer.claim(item, 1 * aq.pageSize()));
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK(aq.appendData(item, 0, 1 * aq.pageSize()));
    }

    // Force the first commit to fail by manipuating the control queue entry.
    aq.ctrl->ctrlq[1] |= CtrlOverlay::CTRLQ_DISCARD_MASK;
    CHECK(!aq.writer.commit(item));
    aq.ctrl->ctrlq[1] &= ~CtrlOverlay::CTRLQ_DISCARD_MASK;

    // Now verify none of the items were committed.
    for (size_t i = 0; i < 1; ++i)
    {
        REQUIRE((aq.ctrl->ctrlq[i] & CtrlOverlay::CTRLQ_COMMIT_MASK));
    }
    for (size_t i = 1; i < 3; ++i)
    {
        REQUIRE(!(aq.ctrl->ctrlq[i] & CtrlOverlay::CTRLQ_COMMIT_MASK));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_MultiItemCommit_when_LastCommitFails_then_StartItemsCommitted, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    CHECK(aq.writer.claim(item, 1 * aq.pageSize()));
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK(aq.appendData(item, 0, 1 * aq.pageSize()));
    }

    // Force the first commit to fail by manipuating the control queue entry.
    aq.ctrl->ctrlq[2] |= CtrlOverlay::CTRLQ_DISCARD_MASK;
    CHECK(!aq.writer.commit(item));
    aq.ctrl->ctrlq[2] &= ~CtrlOverlay::CTRLQ_DISCARD_MASK;

    // Now verify none of the items were committed.
    for (size_t i = 0; i < 2; ++i)
    {
        REQUIRE((aq.ctrl->ctrlq[i] & CtrlOverlay::CTRLQ_COMMIT_MASK));
    }
    for (size_t i = 2; i < 3; ++i)
    {
        REQUIRE(!(aq.ctrl->ctrlq[i] & CtrlOverlay::CTRLQ_COMMIT_MASK));
    }
}



//=============================== End of File ==================================
