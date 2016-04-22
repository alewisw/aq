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

// Runs a basic test with an initial capacity of 'initialCap' followed by
// one or more appends of size1 ... size4.
//
// After all th appends are done the 'expectedCapacity' and 'expectedBufferCount'
// provide the basic parameters for the write item.
static void basicTest(AQTest& aq, size_t expectedCapacity, size_t expectedBufferCount,
    size_t initialCap, size_t size1, size_t size2 = 0, size_t size3 = 0, size_t size4 = 0);

// Runs a queue identifier wrap test by wrapping an item with 'bufferCount' entiries such
// that the first buffer appears 'offsetBeforeWrap' places before the wrap occurs.
static void quidWrapTest(AQTest& aq, size_t bufferCount, size_t offsetBeforeWrap);



//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtExtendable);

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_NonExtendableQueue_when_Append_then_AppendFails, AQ::OPTION_LINK_IDENTIFIER)
{
    char mem[1];
    mem[0] = 'f';
    AQWriterItem witem;
    CHECK(aq.writer.claim(witem, 1));
    REQUIRE(!aq.writer.append(witem, mem, sizeof(mem)));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_EmptyBuffer_when_AppendLessThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 1 * aq.pageSize(), 1, aq.pageSize(), aq.pageSize() - 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_EmptyBuffer_when_AppendExactlyPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 1 * aq.pageSize(), 1, aq.pageSize(), aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_EmptyBuffer_when_AppendMoreThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize() + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_EmptyBuffer_when_AppendExactlyDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize() * 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_EmptyBuffer_when_AppendMoreThanDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize() * 2 + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_PartialBuffer_when_AppendLessThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 1 * aq.pageSize(), 1, aq.pageSize(), 2, aq.pageSize() - 3);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_PartialBuffer_when_AppendExactlyPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 1 * aq.pageSize(), 1, aq.pageSize(), 2, aq.pageSize() - 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_PartialBuffer_when_AppendMoreThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), 2, aq.pageSize() - 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_PartialBuffer_when_AppendExactlyDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), 2, 2 * aq.pageSize() - 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_PartialBuffer_when_AppendMoreThanDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 2, aq.pageSize(), 2, 2 * aq.pageSize() - 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullBuffer_when_AppendLessThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize(), 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullBuffer_when_AppendExactlyPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize(), aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullBuffer_when_AppendMoreThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize(), aq.pageSize() + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullBuffer_when_AppendExactlyDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize(), aq.pageSize() * 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullBuffer_when_AppendMoreThanDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 4 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize(), aq.pageSize() * 2 + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullAndPartialBuffer_when_AppendLessThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize() + 2, aq.pageSize() - 3);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullAndPartialBuffer_when_AppendExactlyPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 2, aq.pageSize(), aq.pageSize() + 2, aq.pageSize() - 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullAndPartialBuffer_when_AppendMoreThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize() + 2, aq.pageSize() - 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullAndPartialBuffer_when_AppendExactlyDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize() + 2, 2 * aq.pageSize() - 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FullAndPartialBuffer_when_AppendMoreThanDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize() + 2, 2 * aq.pageSize() - 3);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoFullBuffers_when_AppendLessThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize(), aq.pageSize(), 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoFullBuffers_when_AppendExactlyPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize(), aq.pageSize(), aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoFullBuffers_when_AppendMoreThanPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 4 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize(), aq.pageSize(), aq.pageSize() + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoFullBuffers_when_AppendExactlyDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 4 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize(), aq.pageSize(), 2 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoFullBuffers_when_AppendMoreThanDoublePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 5 * aq.pageSize(), 3, aq.pageSize(), aq.pageSize(), aq.pageSize(), 2 * aq.pageSize() + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleSizeInitialBuffer_when_AppendLessThanTwoPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 1, 2 * aq.pageSize(), 2 * aq.pageSize() - 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleSizeInitialBuffer_when_AppendExactlyTwoPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 2 * aq.pageSize(), 1, 2 * aq.pageSize(), 2 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleSizeInitialBuffer_when_AppendMoreThanTwoPageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 2, 2 * aq.pageSize(), 2 * aq.pageSize() + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleSizeInitialBuffer_when_AppendExactlyThreePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 3 * aq.pageSize(), 2, 2 * aq.pageSize(), 3 * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_DoubleSizeInitialBuffer_when_AppendMoreThanThreePageSize_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    basicTest(aq, 4 * aq.pageSize(), 2, 2 * aq.pageSize(), 3 * aq.pageSize() + 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_TwoBufferItemStraddlesQuidWrap_when_Retrieve_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    quidWrapTest(aq, 2, 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeBufferItemOneBufferBeforeStraddlesQuidWrap_when_Retrieve_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    quidWrapTest(aq, 3, 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_ThreeBufferItemTwoBuffersBeforeStraddlesQuidWrap_when_Retrieve_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    quidWrapTest(aq, 3, 2);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_FourBufferItemStraddlesQuidWrap_when_Retrieve_then_RetrieveDataMatches, AQ::OPTION_EXTENDABLE)
{
    quidWrapTest(aq, 4, 2);
}

//------------------------------------------------------------------------------
static void basicTest(AQTest& aq, size_t expectedCapacity, size_t expectedBufferCount,
    size_t initialCap, size_t size1, size_t size2, size_t size3, size_t size4)
{
    TraceBuffer *m_trace = aq.trace;
    for (uint32_t startIdx = 0; startIdx < aq.pageCount(); ++startIdx)
    {
        TRACE("Test %u:%u/%u/%u/%u @ %u", initialCap, size1, size2, size3, size4, startIdx);
        while (aq.ctrl->queueRefToIndex(aq.ctrl->headRef) != startIdx)
        {
            aq.advance(1);
        }

        AQWriterItem witem;
        REQUIRE(aq.writer.claim(witem, initialCap));

        size_t pos = 0;
        if (size1)
        {
            REQUIRE(aq.appendData(witem, pos, size1));
            pos += size1;
        }
        if (size2)
        {
            REQUIRE(aq.appendData(witem, pos, size2));
            pos += size2;
        }
        if (size3)
        {
            REQUIRE(aq.appendData(witem, pos, size3));
            pos += size3;
        }
        if (size4)
        {
            REQUIRE(aq.appendData(witem, pos, size4));
            pos += size4;
        }

        size_t totalSize = 0;
        size_t totalCapacity = 0;
        size_t totalCount = 0;
        for (const AQItem *curr = &witem; curr != NULL; curr = curr->next())
        {
            totalSize += curr->size();
            totalCapacity += curr->capacity();
            totalCount++;
        }
        REQUIRE(totalSize == pos);
        REQUIRE(totalCapacity == expectedCapacity);
        REQUIRE(totalCount == expectedBufferCount);
        REQUIRE(aq.writer.commit(witem));

        AQItem ritem;
        REQUIRE(aq.reader.retrieve(ritem));
        REQUIRE(aq.isItemData(ritem, 0, pos));
        aq.reader.release(ritem);
    }
}

//------------------------------------------------------------------------------
static void quidWrapTest(AQTest& aq, size_t bufferCount, size_t offsetBeforeWrap)
{
    size_t wrap = 1 + ((AQItem::QUEUE_IDENTIFIER_MASK & CtrlOverlay::REF_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT);
    aq.advance(aq.pageCount() * wrap - offsetBeforeWrap);

    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    for (size_t bufIdx = 0; bufIdx < bufferCount; ++bufIdx)
    {
        REQUIRE(aq.appendData(witem, bufIdx * aq.pageSize(), aq.pageSize()));
        if (bufIdx > 0)
        {
            if (bufIdx == offsetBeforeWrap)
            {
                REQUIRE(witem.last()->queueIdentifier() < witem.last()->prev()->queueIdentifier());
            }
            else
            {
                REQUIRE(witem.last()->queueIdentifier() > witem.last()->prev()->queueIdentifier());
            }
        }
    }
    REQUIRE(aq.writer.commit(witem));

    AQItem ritem;
    REQUIRE(aq.reader.retrieve(ritem));
    REQUIRE(aq.isItemData(ritem, 0, bufferCount * aq.pageSize()));
    aq.reader.release(ritem);
}



//=============================== End of File ==================================
