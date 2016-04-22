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
TEST_SUITE(UtCrc32LinkId);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemWithLinkId_when_Retrieve_then_Crc32Valid, AQ::OPTION_CRC32 | AQ::OPTION_LINK_IDENTIFIER)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    witem.setLinkIdentifier(0x12345678);
    aq.writer.commit(witem);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(ritem.linkIdentifier() == 0x12345678);
    REQUIRE(ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemWithLinkId_when_Snapshot_then_Crc32Valid, AQ::OPTION_CRC32 | AQ::OPTION_LINK_IDENTIFIER)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    witem.setLinkIdentifier(0x82345679);
    aq.writer.commit(witem);

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(snap[0].linkIdentifier() == 0x82345679);
    REQUIRE(snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ReleasedItemWithLinkId_when_Snapshot_then_Crc32Valid, AQ::OPTION_CRC32 | AQ::OPTION_LINK_IDENTIFIER)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    witem.setLinkIdentifier(0xF2345678);
    aq.writer.commit(witem);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(ritem.linkIdentifier() == 0xF2345678);
    REQUIRE(ritem.isChecksumValid());

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(snap[0].linkIdentifier() == 0xF2345678);
    REQUIRE(snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemWithLinkId_when_LinkIdChangeBeforeRetrieve_then_Crc32Fails, AQ::OPTION_CRC32 | AQ::OPTION_LINK_IDENTIFIER)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    witem.setLinkIdentifier(0x12345678);
    aq.writer.commit(witem);

    aq.ctrl->ctrlq[aq.ctrl->pageCount + 0]++;

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(ritem.linkIdentifier() == 0x12345679);
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemWithLinkId_when_LinkIdChangeBeforeSnapshot_then_Crc32Fails, AQ::OPTION_CRC32 | AQ::OPTION_LINK_IDENTIFIER)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    witem.setLinkIdentifier(0x82345679);
    aq.writer.commit(witem);

    aq.ctrl->ctrlq[aq.ctrl->pageCount + 0]++;

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(snap[0].linkIdentifier() == 0x8234567A);
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ReleasedItemWithLinkId_when_LinkIdChangeBeforeSnapshot_then_Crc32Fails, AQ::OPTION_CRC32 | AQ::OPTION_LINK_IDENTIFIER)
{
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    witem.setLinkIdentifier(0xF2345678);
    aq.writer.commit(witem);

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(ritem.linkIdentifier() == 0xF2345678);
    REQUIRE(ritem.isChecksumValid());

    aq.ctrl->ctrlq[aq.ctrl->pageCount + 0]++;

    AQSnapshot snap(aq.reader, aq.trace);
    REQUIRE(snap.size() == 1);
    REQUIRE(snap[0].linkIdentifier() == 0xF2345679);
    REQUIRE(!snap[0].isChecksumValid());
}




//=============================== End of File ==================================
