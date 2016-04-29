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

#include <stddef.h>
#include <string.h>




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
// Test Cases
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtFormat);

//------------------------------------------------------------------------------
TEST(when_MemoryNull_then_FormatFails)
{
    AQReader q(NULL, 10000);
    REQUIRE(!q.format(2, 100));
    REQUIRE(!q.isFormatted());
    REQUIRE(q.pageCount() == 0);
}

//------------------------------------------------------------------------------
TEST(given_Formatted_when_CorruptFormatMask_then_FormatFails)
{
    unsigned char mem[10000];
    AQReader q(mem, sizeof(mem));
    CHECK(q.format(2, 100, AQ::OPTION_EXTENDABLE));
    CHECK(q.isExtendable());
    mem[0] = 0xFF;
    REQUIRE(!q.isExtendable());
}

//------------------------------------------------------------------------------
TEST(given_Formatted_when_CorruptFormatMask_then_PageSizeZero)
{
    unsigned char mem[10000];
    AQReader q(mem, sizeof(mem));
    CHECK(q.format(2, 100, AQ::OPTION_EXTENDABLE));
    CHECK(q.pageSize() == 4);
    mem[0] = 0xFF;
    REQUIRE(q.pageSize() == 0);
}

//------------------------------------------------------------------------------
TEST(given_Formatted_when_CorruptFormatMask_then_PageCountZero)
{
    unsigned char mem[10000];
    AQReader q(mem, sizeof(mem));
    CHECK(q.format(2, 100, AQ::OPTION_EXTENDABLE));
    CHECK(q.pageCount() > 0);
    mem[0] = 0xFF;
    REQUIRE(q.pageCount() == 0);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeZero_then_FormatFails)
{
    unsigned char mem[10000];

    AQReader q(mem, 0);
    REQUIRE(!q.format(2, 100));
    REQUIRE(!q.isFormatted());
    REQUIRE(q.pageCount() == 0);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeCtrlOverlaySize_then_FormatFails)
{
    unsigned char mem[10000];

    AQReader q(mem, sizeof(CtrlOverlay));
    REQUIRE(!q.format(2, 100));
    REQUIRE(!q.isFormatted());
    REQUIRE(q.pageCount() == 0);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeAllowsOnePage_then_FormatFails)
{
    const int nPages = 1;
    unsigned char mem[sizeof(CtrlOverlay) + (nPages - 1) * sizeof(uint32_t) + nPages * 4];

    AQReader q(mem, sizeof(mem));
    REQUIRE(!q.format(2, 100));
    REQUIRE(!q.isFormatted());
    REQUIRE(q.pageCount() == 0);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeAllowsTwoPages_then_FormatSucceeds)
{
    const int nPages = 2;
    unsigned char mem[sizeof(CtrlOverlay) + (nPages - 1) * sizeof(uint32_t) + nPages * 4];

    AQReader q(mem, sizeof(mem));
    REQUIRE(q.format(2, 100));
    REQUIRE(q.isFormatted());
    REQUIRE(q.pageCount() == nPages);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeExactlyFitsNPages_then_NPagesUsed)
{
    const int nPages = 10;
    unsigned char mem[sizeof(CtrlOverlay) + (nPages - 1) * sizeof(uint32_t) + nPages * 4];

    AQReader q(mem, sizeof(mem));
    q.format(2, 100);
    REQUIRE(q.isFormatted());
    REQUIRE(q.pageCount() == nPages);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeOneLessThanNPages_then_NTake1PagesUsed)
{
    const int nPages = 10;
    unsigned char mem[sizeof(CtrlOverlay) + (nPages - 1) * sizeof(uint32_t) + nPages * 4 - 1];

    AQReader q(mem, sizeof(mem));
    q.format(2, 100);
    REQUIRE(q.isFormatted());
    REQUIRE(q.pageCount() == nPages - 1);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeOneMoreThanNPages_then_NPagesUsed)
{
    const int nPages = 10;
    unsigned char mem[sizeof(CtrlOverlay) + (nPages - 1) * sizeof(uint32_t) + nPages * 4 + 1];

    AQReader q(mem, sizeof(mem));
    q.format(2, 100);
    REQUIRE(q.isFormatted());
    REQUIRE(q.pageCount() == nPages);
}

//------------------------------------------------------------------------------
TEST(when_MemorySizeAllowsMoreThanMaxPages_then_MaxPagesUsed)
{
    const int nPages = CtrlOverlay::PAGE_COUNT_MAX + 1;
    size_t sz = sizeof(CtrlOverlay) + (nPages - 1) * sizeof(uint32_t) + nPages * 4;
    unsigned char *mem = new unsigned char[sz];

    AQReader q(mem, sz);
    q.format(2, 100);
    REQUIRE(q.isFormatted());
    REQUIRE(q.pageCount() == CtrlOverlay::PAGE_COUNT_MAX);

    delete[] mem;
}

//------------------------------------------------------------------------------
TEST(when_QueueFormattedWithDifferentPageSizeAndCount_then_PagesAligned)
{
    for (size_t pageSizeShift = 2; pageSizeShift < 9; ++pageSizeShift)
    {
        size_t alignMask = 1 << CtrlOverlay::PAGE_ALIGN_SHIFT;
        if (pageSizeShift < CtrlOverlay::PAGE_ALIGN_SHIFT)
        {
            alignMask = 1 << pageSizeShift;
        }
        alignMask--;

        size_t memMin = sizeof(CtrlOverlay) + (1U << (pageSizeShift + 2));

        for (size_t memSize = memMin; memSize < memMin + (1U << (pageSizeShift + 4)); ++memSize)
        {
            unsigned char *mem = new unsigned char[memSize];

            AQReader q(mem, memSize);
            q.format(pageSizeShift, 100);
            REQUIRE(q.isFormatted());

            AQWriter p(mem, memSize);
            REQUIRE(p.isFormatted());
            for (size_t i = 0; i < q.pageCount(); ++i)
            {
                AQWriterItem witem;
                REQUIRE(p.claim(witem, 1));
                REQUIRE(((size_t)&witem[0] & alignMask) == 0);
                REQUIRE(p.commit(witem));

                AQItem ritem;
                REQUIRE(q.retrieve(ritem));
                q.release(ritem);
            }

            delete[] mem;
        }
    }
}

//------------------------------------------------------------------------------
TEST(given_QueueFormatted_when_HeaderXrefChanged_then_IndicateUnformatted)
{
    char mem[1000];
    CtrlOverlay *ctrl = (CtrlOverlay *)&mem[0];
    AQReader c(mem, sizeof(mem));
    c.format(5, 100);
    REQUIRE(c.isFormatted());
    ctrl->headerXref |= 1 << 30;
    REQUIRE(!c.isFormatted());
}

//------------------------------------------------------------------------------
TEST(given_QueueFormatted_when_FormatVersionLow_then_IndicateUnformatted)
{
    char mem[1000];
    CtrlOverlay *ctrl = (CtrlOverlay *)&mem[0];
    AQReader c(mem, sizeof(mem));
    c.format(5, 100);
    REQUIRE(c.isFormatted());
    ctrl->formatVersion = 0;
    REQUIRE(!c.isFormatted());
}

//------------------------------------------------------------------------------
TEST(given_QueueFormatted_when_FormatVersionHigh_then_IndicateUnformatted)
{
    char mem[1000];
    CtrlOverlay *ctrl = (CtrlOverlay *)&mem[0];
    AQReader c(mem, sizeof(mem));
    c.format(5, 100);
    REQUIRE(c.isFormatted());
    ctrl->formatVersion++;
    REQUIRE(!c.isFormatted());
}

//------------------------------------------------------------------------------
TEST(given_QueueFormatted_when_OptionInvalid_then_IndicateUnformatted)
{
    char mem[1000];
    CtrlOverlay *ctrl = (CtrlOverlay *)&mem[0];
    AQReader c(mem, sizeof(mem));
    c.format(5, 100);
    REQUIRE(c.isFormatted());
    ctrl->options |= 1 << 31;
    REQUIRE(!c.isFormatted());
}

//------------------------------------------------------------------------------
TEST(given_QueueUnformatted_when_Claim_then_Fails)
{
    // First we create a valid queue and claim an item; then we memset() the
    // memory to clear the formatted status.
    char mem[1000];
    AQReader c(mem, sizeof(mem));
    AQWriter p(mem, sizeof(mem));
    c.format(2, 100);

    AQWriterItem witem;
    memset(mem, 0xCD, offsetof(CtrlOverlay, headerXref) + sizeof(uint32_t));
    REQUIRE_EXCEPTION(p.claim(witem, 1), AQUnformattedException);
}

//------------------------------------------------------------------------------
TEST(given_QueueUnformatted_when_Retrieve_then_Fails)
{
    // First we create a valid queue and claim an item; then we memset() the
    // memory to clear the formatted status.
    char mem[1000];
    AQReader c(mem, sizeof(mem));
    AQWriter p(mem, sizeof(mem));
    c.format(2, 100);

    AQWriterItem witem;
    REQUIRE(p.claim(witem, 1));
    REQUIRE(p.commit(witem));
    AQItem ritem;
    memset(mem, 0xCD, offsetof(CtrlOverlay, headerXref) + sizeof(uint32_t));
    REQUIRE_EXCEPTION(c.retrieve(ritem), AQUnformattedException);
}

//------------------------------------------------------------------------------
TEST(given_QueueFormatted_when_ProducerMemoryTooSmall_then_ProducerUnformatted)
{
    char mem[1000];
    AQReader c(mem, sizeof(mem));
    c.format(2, 100);

    AQWriter p(mem, sizeof(mem) - 1);

    REQUIRE(c.isFormatted());
    REQUIRE(!p.isFormatted());
}

//------------------------------------------------------------------------------
TEST(given_QueueFormatted_when_ProducerMemoryTooLarge_then_ProducerFormatted)
{
    char mem[1000];
    AQReader c(mem, sizeof(mem));
    c.format(2, 100);

    AQWriter p(mem, sizeof(mem) + 1);

    REQUIRE(c.isFormatted());
    REQUIRE(p.isFormatted());
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFormatted_when_Format_then_TimeoutTrackingStateReset)
{
    AQWriterItem witem;
    AQItem ritem;

    // Force timeout timer to start and expire.
    CHECK(aq.writer.claim(witem, 1));
    CHECK(!aq.reader.retrieve(ritem));
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);

    aq.reformat();
    CHECK(aq.writer.claim(witem, 1));
    REQUIRE(!aq.reader.retrieve(ritem));
}

//------------------------------------------------------------------------------
AQTEST(given_QueueFormatted_when_Format_then_RetrievedStateReset)
{
    AQWriterItem witem;
    AQItem ritem;

    // Force timeout timer to start and expire.
    CHECK(aq.writer.claim(witem, 1));
    CHECK(aq.writer.commit(witem));
    CHECK(aq.reader.retrieve(ritem));

    aq.reformat();
    CHECK(aq.writer.claim(witem, 1));
    REQUIRE(!aq.reader.retrieve(ritem));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_QueueFormatted_when_Format_then_LinkProcessorStateReset, AQ::OPTION_EXTENDABLE)
{
    AQWriterItem witem;
    AQItem ritem;

    // Commit the first two pages of an item to the queue.
    CHECK(aq.writer.claim(witem, 1));
    CHECK(aq.appendData(witem, 0, 2 * aq.pageSize()));
    CHECK(aq.appendData(witem, 0, aq.pageSize()));
    CHECK(aq.writer.commitExtendable(witem, 0, 1));
    witem.clear();

    // Read it to populate the link processor.
    CHECK(!aq.reader.retrieve(ritem));

    // Should clear the link processor.
    aq.reformat();
    
    // Recreate the item and commit the last part.
    CHECK(aq.writer.claim(witem, 1));
    CHECK(aq.appendData(witem, 0, 2 * aq.pageSize()));
    CHECK(aq.appendData(witem, 0, aq.pageSize()));
    CHECK(aq.writer.commitExtendable(witem, 2, 2));

    // Read it - must not return a result.
    REQUIRE(!aq.reader.retrieve(ritem));
}




//=============================== End of File ==================================
