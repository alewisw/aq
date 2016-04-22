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
TEST_SUITE(UtExtendableCrc32);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ItemDataValid_when_ItemRetrieved_then_ChecksumValid, AQ::OPTION_EXTENDABLE | AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, aq.pageSize()));
    REQUIRE(aq.appendData(witem, aq.pageSize(), aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 2));
    REQUIRE(aq.writer.commit(witem));

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, 2 * aq.pageSize() + 2));
    REQUIRE(ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ItemDataCorrupted_when_ItemRetrieved_then_ChecksumInvalid, AQ::OPTION_EXTENDABLE | AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, aq.pageSize()));
    REQUIRE(aq.appendData(witem, aq.pageSize(), aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 2));
    
    const AQItem &last = *witem.last();
    const unsigned char& data = last[last.size() - 1];

    REQUIRE(aq.writer.commit(witem));

    *((unsigned char *)&data) = data + 1;

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ItemDataCorrupted_when_ItemRetrieved_then_TruncatedAfterFailedItem, AQ::OPTION_EXTENDABLE | AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, aq.pageSize()));
    REQUIRE(aq.appendData(witem, aq.pageSize(), aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 2));

    const AQItem &last = *witem.last();
    const unsigned char& data = last[last.size() - 1];

    REQUIRE(aq.writer.commit(witem));

    *((unsigned char *)&data) = data + 1;

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isPartialItemData(ritem, 0, 2 * aq.pageSize() + 2, 2));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ItemDataValid_when_Snapshot_then_ChecksumValid, AQ::OPTION_EXTENDABLE | AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, aq.pageSize()));
    REQUIRE(aq.appendData(witem, aq.pageSize(), aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 2));
    REQUIRE(aq.writer.commit(witem));

    AQSnapshot snap(aq.writer, aq.trace);

    REQUIRE(snap.size() == 1);
    REQUIRE(snap[0].isChecksumValid());
    REQUIRE(aq.isItemData(snap[0], 0, 2 * aq.pageSize() + 2));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ItemDataCorrupted_when_Snapshot_then_ChecksumInvalid, AQ::OPTION_EXTENDABLE | AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, aq.pageSize()));
    REQUIRE(aq.appendData(witem, aq.pageSize(), aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 2));

    const AQItem &last = *witem.last();
    const unsigned char& data = last[last.size() - 1];

    REQUIRE(aq.writer.commit(witem));

    *((unsigned char *)&data) = data + 1;

    AQSnapshot snap(aq.writer, aq.trace);

    REQUIRE(snap.size() == 1);
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ItemDataCorrupted_when_Snapshot_then_TruncatedAfterFailedItem, AQ::OPTION_EXTENDABLE | AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.appendData(witem, 0, aq.pageSize()));
    REQUIRE(aq.appendData(witem, aq.pageSize(), aq.pageSize()));
    REQUIRE(aq.appendData(witem, 2 * aq.pageSize(), 2));

    const AQItem &last = *witem.last();
    const unsigned char& data = last[last.size() - 1];

    REQUIRE(aq.writer.commit(witem));

    *((unsigned char *)&data) = data + 1;

    AQSnapshot snap(aq.writer, aq.trace);

    REQUIRE(snap.size() == 1);
    REQUIRE(aq.isPartialItemData(snap[0], 0, 2 * aq.pageSize() + 2, 2));
}




//=============================== End of File ==================================
