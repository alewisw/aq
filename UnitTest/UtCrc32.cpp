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
TEST_SUITE(UtCrc32);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItem_when_Retrieve_then_Crc32Valid, AQ::OPTION_CRC32)
{
    aq.enqueue(1);

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultipleItems_when_Retrieve_then_Crc32Valid, AQ::OPTION_CRC32)
{
    for (size_t i = 0; i < aq.pageCount() * 3 + 2; ++i)
    {
        AQWriterItem witem;
        REQUIRE(aq.writer.claim(witem, aq.pageSize()));
        for (int j = 0; j < 4; ++j)
        {
            witem[j]++;
            if (witem[j] != 0)
            {
                break;
            }
        }
        REQUIRE(aq.writer.commit(witem));

        AQItem ritem;
        REQUIRE(aq.reader.retrieve(ritem));
        REQUIRE(ritem.isCommitted());
        REQUIRE(ritem.isChecksumValid());
        aq.reader.release(ritem);
    }
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItem_when_Retrieve_then_Crc32Valid, AQ::OPTION_CRC32)
{
    aq.enqueue(1, aq.pageSize() * 4);

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitJustAfterWaste_when_Retrieve_then_Crc32Valid, AQ::OPTION_CRC32)
{
    aq.advance(7);
    aq.enqueue(1);
    aq.enqueue(1, aq.pageSize() * 4);
    aq.enqueue(1);

    for (int i = 0; i < 3; ++i)
    {
        AQItem ritem;
        aq.reader.retrieve(ritem);
        REQUIRE(ritem.isCommitted());
        REQUIRE(ritem.isChecksumValid());
    }
}


//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST_FORMAT(given_CommitFailJustBeforeCtrlqUpdate_when_RetrieveIncomplete_then_Crc32Valid, AQ::OPTION_CRC32)
{
    // Cause an entry with valid CRC but incomplete data to appear.
    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::CommitBeforeWriteCtrl);
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    try
    {
        aq.writer.commit(witem);
    }
    catch (...)
    {
    }
    aq.enqueue(9);

    AQItem ritem;
    aq.reader.retrieve(ritem);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.reader.retrieve(ritem);
    REQUIRE(!ritem.isCommitted());
    REQUIRE(ritem.isChecksumValid());
}
#endif

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemMemoryChange_when_Retrieve_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    unsigned char *ptr = &witem[0];
    ptr[2] = 0xF1;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[2] = 0xF2;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItemMemoryChangeFirstPage_when_Retrieve_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    unsigned char *ptr = &witem[0];
    ptr[2] = 0x6A;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[2] = 0xB7;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItemMemoryChangeMidPage_when_Retrieve_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    unsigned char *ptr = &witem[0];
    ptr[aq.pageSize() + 1] = 0x91;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[aq.pageSize() + 1] = 0x7F;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItemMemoryChangeLastPage_when_Retrieve_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    unsigned char *ptr = &witem[0];
    ptr[3 * aq.pageSize() - 1] = 0xFE;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[3 * aq.pageSize() - 1] = 0x1A;

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemCtrlqSizeChange_when_Retrieve_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 4));
    REQUIRE(aq.writer.commit(witem));

    // Bottom bits are length.
    aq.ctrl->ctrlq[0] = (aq.ctrl->ctrlq[0] & ~CtrlOverlay::CTRLQ_SIZE_MASK) | (aq.pageSize() * 4 - 1);
    
    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ClaimItemIdenticalToPreviousInSamePosition_when_RetrieveIncomplete_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, 2));
    witem[0] = 0x55;
    witem[1] = 0xAA;
    REQUIRE(aq.writer.commit(witem));

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(ritem.isChecksumValid());
    aq.reader.release(ritem);

    aq.advance(aq.pageCount() - 1);

    REQUIRE(aq.writer.claim(witem, 2));
    REQUIRE(witem[0] == 0x55);
    REQUIRE(witem[1] == 0xAA);

    aq.enqueue(9);

    aq.reader.retrieve(ritem);
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    aq.reader.retrieve(ritem);
    REQUIRE(!ritem.isCommitted());
    REQUIRE(!ritem.isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItem_when_Snapshot_then_Crc32Valid, AQ::OPTION_CRC32)
{
    aq.enqueue(1);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItem_when_Snapshot_then_Crc32Valid, AQ::OPTION_CRC32)
{
    aq.enqueue(1, aq.pageSize() * 4);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST_FORMAT(given_CommitFailJustBeforeCtrlqUpdate_when_Snapshot_then_Crc32Valid, AQ::OPTION_CRC32)
{
    // Cause an entry with valid CRC but incomplete data to appear.
    ThrowAction ta(aq.writer);
    ta.attach(aq.writer, AQWriter::CommitBeforeWriteCtrl);
    AQWriterItem witem;
    aq.writer.claim(witem, aq.pageSize());
    try
    {
        aq.writer.commit(witem);
    }
    catch (...)
    {
    }

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(!snap[0].isCommitted());
    REQUIRE(snap[0].isChecksumValid());
}
#endif

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemMemoryChange_when_Snapshot_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    unsigned char *ptr = &witem[0];
    ptr[2] = 0xF1;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[2] = 0xF2;

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItemMemoryChangeFirstPage_when_Snapshot_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    unsigned char *ptr = &witem[0];
    ptr[2] = 0x6A;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[2] = 0xB7;

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItemMemoryChangeMidPage_when_Snapshot_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    unsigned char *ptr = &witem[0];
    ptr[aq.pageSize() + 1] = 0x91;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[aq.pageSize() + 1] = 0x7F;

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitMultiPageItemMemoryChangeLastPage_when_Snapshot_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    unsigned char *ptr = &witem[0];
    ptr[3 * aq.pageSize() - 1] = 0xFE;
    REQUIRE(aq.writer.commit(witem));
    
    ptr[3 * aq.pageSize() - 1] = 0x1A;

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_CommitItemCtrlqSizeChange_when_Snapshot_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 4));
    REQUIRE(aq.writer.commit(witem));

    // Bottom bits are length.
    aq.ctrl->ctrlq[0] = (aq.ctrl->ctrlq[0] & ~CtrlOverlay::CTRLQ_SIZE_MASK) | (aq.pageSize() * 4 - 1);
    
    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(snap[0].isCommitted());
    REQUIRE(!snap[0].isChecksumValid());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ClaimItemIdenticalToPreviousInSamePosition_when_Snapshot_then_Crc32Fails, AQ::OPTION_CRC32)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, 2));
    witem[0] = 0x55;
    witem[1] = 0xAA;
    REQUIRE(aq.writer.commit(witem));

    AQItem ritem;
    aq.reader.retrieve(ritem);
    REQUIRE(ritem.isCommitted());
    REQUIRE(ritem.isChecksumValid());
    aq.reader.release(ritem);

    aq.advance(aq.pageCount() - 1);

    REQUIRE(aq.writer.claim(witem, 2));
    REQUIRE(witem[0] == 0x55);
    REQUIRE(witem[1] == 0xAA);

    AQSnapshot snap(aq.reader, aq.trace);

    REQUIRE(!snap[10].isCommitted());
    REQUIRE(!snap[10].isChecksumValid());
}



//=============================== End of File ==================================
