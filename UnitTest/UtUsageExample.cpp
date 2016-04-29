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

// The AQ specifically designed for verifying the USAGE.md example.
class UsageAQ
{
public:
    UsageAQ(void)
        : ctrl((CtrlOverlay *)mem)
        , writer(mem, sizeof(mem), m_tm.createBuffer("wrt"))
        , reader(mem, sizeof(mem), m_tm.createBuffer("rdr"))
    {
        CHECK(reader.format(0, AQTest::COMMIT_TIMEOUT_MS - 25));

        AQWriterItem item;
        for (size_t i = 0; i < 11; ++i)
        {
            CHECK(writer.claim(item, 1));
            CHECK(writer.commit(item));
            CHECK(reader.retrieve(item));
            reader.release(item);
        }

        // Index 11, 3 pages released, AA AA AA
        AQWriterItem item11;
        CHECK(writer.claim(item11, 3));
        item11[0] = 0xAA;
        item11[1] = 0xAA;
        item11[2] = 0xAA;
        CHECK(writer.commit(item11));
        CHECK(reader.retrieve(item11));
        CHECK(item11[0] == 0xAA);
        CHECK(item11[1] == 0xAA);
        CHECK(item11[2] == 0xAA);

        // Index 0, 2 pages released, BB BB
        CHECK(writer.claim(item0, 2));
        item0[0] = 0xBB;
        item0[1] = 0xBB;
        CHECK(writer.commit(item0));
        CHECK(reader.retrieve(item0));
        CHECK(item0[0] == 0xBB);
        CHECK(item0[1] == 0xBB);
        reader.release(item0);

        // Index 2, 2 pages incomplete released, cc cc
        AQWriterItem item2;
        CHECK(writer.claim(item2, 2));
        item2[0] = 0xcc;
        item2[1] = 0xcc;
        CHECK(!reader.retrieve(item2));

        // Index 4, 1 page committed, DD
        CHECK(writer.claim(item4, 1));
        item4[0] = 0xDD;

        // Index 5, 2 pages released, EE EE
        AQWriterItem item5;
        CHECK(writer.claim(item5, 2));
        item5[0] = 0xEE;
        item5[1] = 0xEE;
        CHECK(writer.commit(item5));
        CHECK(reader.retrieve(item5));
        CHECK(item5[0] == 0xEE);
        CHECK(item5[1] == 0xEE);
        reader.release(item5);

        // Index 7, 2 pages retrieved, FF FF
        CHECK(writer.claim(item7, 2));
        item7[0] = 0xFF;
        item7[1] = 0xFF;
        CHECK(writer.commit(item7));
        CHECK(reader.retrieve(item7));
        CHECK(item7[0] == 0xFF);
        CHECK(item7[1] == 0xFF);

        // Index 9, 1 page claimed, gg
        CHECK(writer.claim(item9, 1));
        item9[0] = 0x00;

        // Make item 2 incomplete.
        Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
        CHECK(reader.retrieve(item2));
        CHECK(item2[0] == 0xcc);
        CHECK(item2[1] == 0xcc);
        reader.release(item2);

        // Release the tail to move us into position.
        reader.release(item11);

        // Index 10, 1 page committed, HH
        CHECK(writer.claim(item10, 1));
        item10[0] = 0x11;
        CHECK(writer.commit(item10));
    
        // Commit item 4.
        CHECK(writer.commit(item4));
    }

    ~UsageAQ(void)
    {
        if (TEST_IS_FAILING())
        {
            m_tm.write(cout);
        }
    }

    AQSnapshot requireSnapshot(size_t firstOffset, size_t itemCount)
    {
        AQSnapshot snap(reader, m_tm.createBuffer("snp"));
        REQUIRE(snap.size() == itemCount);

        size_t i = 0;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 3);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 0xAA);
            REQUIRE(snap[i - firstOffset][1] == 0xAA);
            REQUIRE(snap[i - firstOffset][2] == 0xAA);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 0xBB);
            REQUIRE(snap[i - firstOffset][1] == 0xBB);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(!snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 0xcc);
            REQUIRE(snap[i - firstOffset][1] == 0xcc);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 0xDD);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 0xEE);
            REQUIRE(snap[i - firstOffset][0] == 0xEE);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 0xFF);
            REQUIRE(snap[i - firstOffset][0] == 0xFF);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(!snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 0x00);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 0x11);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(snap[i - firstOffset][0] == 0x22);
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 5);
            REQUIRE(snap[i - firstOffset][0] == 0x33);
            REQUIRE(snap[i - firstOffset][1] == 0x33);
            REQUIRE(snap[i - firstOffset][2] == 0x33);
            REQUIRE(snap[i - firstOffset][3] == 0x33);
            REQUIRE(snap[i - firstOffset][4] == 0x33);
        }
        return snap;
    }

    void step1Claim1Page(void)
    {
        CHECK(writer.claim(item11, 1));
        item11[0] = 0x22;
    }

    void step2Commit1Page(void)
    {
        CHECK(writer.commit(item11));
    }

    void step3Retrieve1Page(void)
    {
        CHECK(reader.retrieve(item4));
    }

    void step4Release1Page(void)
    {
        reader.release(item4);
    }

    void step5Retrieve1Page(void)
    {
        CHECK(reader.retrieve(item10));
    }

    void step6Retrieve1Page(void)
    {
        CHECK(reader.retrieve(item11));
    }


    void step7Claim5Pages(void)
    {
        CHECK(writer.claim(item0, 5));
        item0[0] = 0x33;
        item0[1] = 0x33;
        item0[2] = 0x33;
        item0[3] = 0x33;
        item0[4] = 0x33;
    }

    void step8Retrieve1Page(void)
    {
        Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
        CHECK(reader.retrieve(item9));
    }

    // The memory to use.
    unsigned char mem[sizeof(aq::CtrlOverlay) + 14 * sizeof(uint32_t) + 15];

    // The dummy trace manager.
    TraceManager m_tm;

    // The control overlay.
    CtrlOverlay *ctrl;

    // The writer to use for the tests.
    AQWriter writer;

    // The reader to use for the tests.
    AQReader reader;

    // The retrieved item at index 0.
    AQWriterItem item0;

    // The retrieved item at index 4.
    AQWriterItem item4;

    // The retrieved item at index 7.
    AQWriterItem item7;

    // The claimed item at index 9.
    AQWriterItem item9;

    // The claimed item at index 10.
    AQWriterItem item10;

    // The claimed item at index 11.
    AQWriterItem item11;
};



//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE_FIRST(UtUsageExample);

//------------------------------------------------------------------------------
TEST(given_UsageExample_when_InitialState_then_ContentMatchesExpected)
{
    UsageAQ aq;

    REQUIRE(aq.reader.pageSize() == 1);
    REQUIRE(aq.reader.pageCount() == 15);
    
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 11);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 4);
    REQUIRE(aq.reader.availableSize() == 4);
    
    AQSnapshot snap = aq.requireSnapshot(0, 8);
    REQUIRE(!snap[3].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep0_when_Claim1Page_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 12);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 4);
    REQUIRE(aq.reader.availableSize() == 3);

    AQSnapshot snap = aq.requireSnapshot(1, 8);
    REQUIRE(!snap[2].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
    REQUIRE(!snap[7].isCommitted());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep1_when_Commit1Page_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 12);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 4);
    REQUIRE(aq.reader.availableSize() == 3);

    AQSnapshot snap = aq.requireSnapshot(1, 8);
    REQUIRE(!snap[2].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
    REQUIRE(snap[7].isCommitted());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep2_when_Retrieve1Page_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();
    aq.step3Retrieve1Page();

    REQUIRE(aq.item4[0] == 0xDD);

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 12);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 4);
    REQUIRE(aq.reader.availableSize() == 3);

    AQSnapshot snap = aq.requireSnapshot(1, 8);
    REQUIRE(!snap[2].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
    REQUIRE(snap[7].isCommitted());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep3_when_Release1Page_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();
    aq.step3Retrieve1Page();
    aq.step4Release1Page();

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 12);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 7);
    REQUIRE(aq.reader.availableSize() == 6);

    AQSnapshot snap = aq.requireSnapshot(1, 8);
    REQUIRE(snap[2].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
    REQUIRE(snap[7].isCommitted());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep4_when_RetrievePage_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();
    aq.step3Retrieve1Page();
    aq.step4Release1Page();
    aq.step5Retrieve1Page();

    REQUIRE(aq.item10[0] == 0x11);

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 12);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 7);
    REQUIRE(aq.reader.availableSize() == 6);

    AQSnapshot snap = aq.requireSnapshot(1, 8);
    REQUIRE(snap[2].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
    REQUIRE(snap[7].isCommitted());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep5_when_RetrievePage_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();
    aq.step3Retrieve1Page();
    aq.step4Release1Page();
    aq.step5Retrieve1Page();
    aq.step6Retrieve1Page();

    REQUIRE(aq.item11[0] == 0x22);

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 12);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 7);
    REQUIRE(aq.reader.availableSize() == 6);

    AQSnapshot snap = aq.requireSnapshot(1, 8);
    REQUIRE(snap[2].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isReleased());
    REQUIRE(!snap[6].isReleased());
    REQUIRE(!snap[7].isReleased());
    REQUIRE(snap[7].isCommitted());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep6_when_Claim5Pages_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();
    aq.step3Retrieve1Page();
    aq.step4Release1Page();
    aq.step5Retrieve1Page();
    aq.step6Retrieve1Page();
    aq.step7Claim5Pages();

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 5);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 7);
    REQUIRE(aq.reader.availableSize() == 1);

    AQSnapshot snap = aq.requireSnapshot(4, 6);
    REQUIRE(!snap[1].isReleased());
    REQUIRE(!snap[2].isReleased());
    REQUIRE(!snap[3].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isCommitted());
    REQUIRE(!snap[5].isReleased());
}

//------------------------------------------------------------------------------
TEST(given_UsageExampleAtStep7_when_Retrieve1Page_then_ContentMatchesExpected)
{
    UsageAQ aq;

    aq.step1Claim1Page();
    aq.step2Commit1Page();
    aq.step3Retrieve1Page();
    aq.step4Release1Page();
    aq.step5Retrieve1Page();
    aq.step6Retrieve1Page();
    aq.step7Claim5Pages();
    aq.step8Retrieve1Page();

    REQUIRE(aq.item9[0] == 0x00);
    REQUIRE(!aq.item9.isCommitted());

    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->headRef) == 5);
    REQUIRE(aq.ctrl->queueRefToIndex(aq.ctrl->tailRef) == 7);
    REQUIRE(aq.reader.availableSize() == 1);

    AQSnapshot snap = aq.requireSnapshot(4, 6);
    REQUIRE(!snap[1].isReleased());
    REQUIRE(!snap[2].isReleased());
    REQUIRE(!snap[3].isReleased());
    REQUIRE(!snap[4].isReleased());
    REQUIRE(!snap[5].isCommitted());
    REQUIRE(!snap[5].isReleased());
}


//=============================== End of File ==================================
