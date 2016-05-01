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
TEST_SUITE(UtExtendableSnapshot);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_EmptyQueue_when_Snapshot_then_ZeroItems, AQ::OPTION_EXTENDABLE)
{
    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_SinglePartialBufferItemCommitted_when_Snapshot_then_ItemUnreleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize() / 2));
    aq.appendData(item, 0, aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize() / 2));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_SinglePartialBufferItemReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize() / 2));
    aq.appendData(item, 0, aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() / 2));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize() / 2, true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_SingleFullBufferItemCommitted_when_Snapshot_then_ItemUnreleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, aq.pageSize());
    REQUIRE(aq.writer.commit(item));

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_SingleFullBufferItemReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, aq.pageSize());
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize()));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize(), true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoublePartialBufferItemCommitted_when_Snapshot_then_ItemUnreleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 3 * aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 3 * aq.pageSize() / 2));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoublePartialBufferItemReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 3 * aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 3 * aq.pageSize() / 2));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 3 * aq.pageSize() / 2, true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoublePartialBufferItemPartReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 3 * aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 3 * aq.pageSize() / 2));
    aq.reader.releaseExtendable(ritem, 0, 0);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 3 * aq.pageSize() / 2, true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleFullBufferItemCommitted_when_Snapshot_then_ItemUnreleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    REQUIRE(aq.writer.commit(item));

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 2 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleFullBufferItemReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 2 * aq.pageSize()));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 2 * aq.pageSize(), true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleFullBufferItemPartReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 2 * aq.pageSize()));
    aq.reader.releaseExtendable(ritem, 0, 0);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 2 * aq.pageSize(), true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TriplePartialBufferItemCommitted_when_Snapshot_then_ItemUnreleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    aq.appendData(item, 2 * aq.pageSize(), aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 5 * aq.pageSize() / 2));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TriplePartialBufferItemReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    aq.appendData(item, 2 * aq.pageSize(), aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 5 * aq.pageSize() / 2));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 5 * aq.pageSize() / 2, true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TriplePartialBufferItemPartReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    aq.appendData(item, 2 * aq.pageSize(), aq.pageSize() / 2);
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 5 * aq.pageSize() / 2));
    aq.reader.releaseExtendable(ritem, 0, 0);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 5 * aq.pageSize() / 2, true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TripleFullBufferItemCommitted_when_Snapshot_then_ItemUnreleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    aq.appendData(item, 2 * aq.pageSize(), aq.pageSize());
    REQUIRE(aq.writer.commit(item));

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 3 * aq.pageSize()));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TripleFullBufferItemReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    aq.appendData(item, 2 * aq.pageSize(), aq.pageSize());
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 3 * aq.pageSize()));
    aq.reader.release(ritem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 3 * aq.pageSize(), true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TripleFullBufferItemPartReleased_when_Snapshot_then_ItemReleased, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem item;
    REQUIRE(aq.writer.claim(item, aq.pageSize()));
    aq.appendData(item, 0, 2 * aq.pageSize());
    aq.appendData(item, 2 * aq.pageSize(), aq.pageSize());
    REQUIRE(aq.writer.commit(item));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 3 * aq.pageSize()));
    aq.reader.releaseExtendable(ritem, 0, 0);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isItemData(snap[0], 0, 3 * aq.pageSize(), true));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueDoublePartialFirstBufferPresent_when_Snapshot_then_ItemReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, aq.pageSize() + 1);
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() + 1));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 2);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is returned as well as all aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 1);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize() + 1, true));
    for (size_t i = 1; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i - 1));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueDoublePartialFirstBufferDiscarded_when_Snapshot_then_ItemNotReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, aq.pageSize() + 1);
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() + 1));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 1);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is not returned - just the aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 1);
    for (size_t i = 0; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueDoubleFullFirstBufferPresent_when_Snapshot_then_ItemReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, 2 * aq.pageSize());
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 2 * aq.pageSize()));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 2);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is returned as well as all aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 1);
    REQUIRE(aq.isItemData(snap[0], 0, 2 * aq.pageSize(), true));
    for (size_t i = 1; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i - 1));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueDoubleFullFirstBufferDiscarded_when_Snapshot_then_ItemNotReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, 2 * aq.pageSize());
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 2 * aq.pageSize()));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 1);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is not returned - just the aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 1);
    for (size_t i = 0; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueTriplePartialFirstBufferPresent_when_Snapshot_then_ItemReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, aq.pageSize() * 2);
    aq.appendData(witem, aq.pageSize() * 2, 1);
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() * 2 + 1));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 3);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is returned as well as all aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 2);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize() * 2 + 1, true));
    for (size_t i = 1; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i - 1));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueTriplePartialFirstBufferDiscarded_when_Snapshot_then_ItemNotReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, aq.pageSize() * 2);
    aq.appendData(witem, aq.pageSize() * 2, 1);
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() * 2 + 1));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 2);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is not returned - just the aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 2);
    for (size_t i = 0; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueTripleFullFirstBufferPresent_when_Snapshot_then_ItemReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, aq.pageSize() * 2);
    aq.appendData(witem, aq.pageSize() * 2, aq.pageSize());
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() * 3));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 3);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is returned as well as all aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 2);
    REQUIRE(aq.isItemData(snap[0], 0, aq.pageSize() * 3, true));
    for (size_t i = 1; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i - 1));
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullQueueTripleFullFirstBufferDiscarded_when_Snapshot_then_ItemNotReturned, AQ::OPTION_EXTENDABLE)
{
    // Put the item in the queue.
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    aq.appendData(witem, 0, aq.pageSize() * 2);
    aq.appendData(witem, aq.pageSize() * 2, aq.pageSize());
    REQUIRE(aq.writer.commit(witem));
    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, aq.pageSize() * 3));
    aq.reader.release(ritem);

    // Now fill the rest of the queue.
    aq.enqueue(aq.pageCount() - 2);

    // Take the snapshot.
    AQSnapshot snap(aq.reader, aq.trace);

    // Verify the item is not returned - just the aq.enqueued items.
    REQUIRE(snap.size() == aq.pageCount() - 2);
    for (size_t i = 0; i < snap.size(); ++i)
    {
        REQUIRE(aq.isEnqueueItemData(snap[i], i));
    }
}




//=============================== End of File ==================================
