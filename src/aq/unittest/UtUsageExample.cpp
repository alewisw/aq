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

#include "AQHeapMemory.h"

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
        : m_sm(mem, sizeof(mem))
        , ctrl((CtrlOverlay *)mem)
        , writer(m_sm, m_tm.createBuffer("wrt"))
        , reader(m_sm, m_tm.createBuffer("rdr"))
    {
        CHECK(reader.format(0, AQTest::COMMIT_TIMEOUT_MS - 25));

        initQueue(writer, reader);
    }

    void initQueue(AQWriter& writer, AQReader& reader)
    {

        AQWriterItem item;
        for (size_t i = 0; i < 11; ++i)
        {
            CHECK(writer.claim(item, 1 * writer.pageSize()));
            CHECK(writer.commit(item));
            CHECK(reader.retrieve(item));
            reader.release(item);
        }

        // Index 11, 3 pages released, AA AA AA
        AQWriterItem item11;
        CHECK(writer.claim(item11, 3 * writer.pageSize()));
        item11[0] = 'A';
        item11[1] = 'A';
        item11[2] = 'A';
        CHECK(writer.commit(item11));
        CHECK(reader.retrieve(item11));
        CHECK(item11[0] == 'A');
        CHECK(item11[1] == 'A');
        CHECK(item11[2] == 'A');

        // Index 0, 2 pages released, BB BB
        CHECK(writer.claim(item0, 2 * writer.pageSize()));
        item0[0] = 'B';
        item0[1] = 'B';
        CHECK(writer.commit(item0));
        CHECK(reader.retrieve(item0));
        CHECK(item0[0] == 'B');
        CHECK(item0[1] == 'B');
        reader.release(item0);

        // Index 2, 2 pages incomplete released, cc cc
        AQWriterItem item2;
        CHECK(writer.claim(item2, 2 * writer.pageSize()));
        item2[0] = 'c';
        item2[1] = 'c';
        CHECK(!reader.retrieve(item2));

        // Index 4, 1 page committed, DD
        CHECK(writer.claim(item4, 1 * writer.pageSize()));
        item4[0] = 'D';

        // Index 5, 2 pages released, EE EE
        AQWriterItem item5;
        CHECK(writer.claim(item5, 2 * writer.pageSize()));
        item5[0] = 'E';
        item5[1] = 'E';
        CHECK(writer.commit(item5));
        CHECK(reader.retrieve(item5));
        CHECK(item5[0] == 'E');
        CHECK(item5[1] == 'E');
        reader.release(item5);

        // Index 7, 2 pages retrieved, FF FF
        CHECK(writer.claim(item7, 2 * writer.pageSize()));
        item7[0] = 'F';
        item7[1] = 'F';
        CHECK(writer.commit(item7));
        CHECK(reader.retrieve(item7));
        CHECK(item7[0] == 'F');
        CHECK(item7[1] == 'F');

        // Index 9, 1 page claimed, gg
        CHECK(writer.claim(item9, 1 * writer.pageSize()));
        item9[0] = 'g';

        // Make item 2 incomplete.
        Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
        CHECK(reader.retrieve(item2));
        CHECK(item2[0] == 'c');
        CHECK(item2[1] == 'c');
        reader.release(item2);

        // Release the tail to move us into position.
        reader.release(item11);

        // Index 10, 1 page committed, HH
        CHECK(writer.claim(item10, 1 * writer.pageSize()));
        item10[0] = 'H';
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
            REQUIRE(snap[i - firstOffset][0] == 'A');
            REQUIRE(snap[i - firstOffset][1] == 'A');
            REQUIRE(snap[i - firstOffset][2] == 'A');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 'B');
            REQUIRE(snap[i - firstOffset][1] == 'B');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(!snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 'c');
            REQUIRE(snap[i - firstOffset][1] == 'c');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 'D');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset].isReleased());
            REQUIRE(snap[i - firstOffset][0] == 'E');
            REQUIRE(snap[i - firstOffset][0] == 'E');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 2);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 'F');
            REQUIRE(snap[i - firstOffset][0] == 'F');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(!snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 'g');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(snap[i - firstOffset].isCommitted());
            REQUIRE(snap[i - firstOffset][0] == 'H');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 1);
            REQUIRE(snap[i - firstOffset][0] == 'I');
        }

        i++;
        if (firstOffset <= i && i - firstOffset < snap.size())
        {
            REQUIRE(snap[i - firstOffset].size() == 5);
            REQUIRE(snap[i - firstOffset][0] == 'j');
            REQUIRE(snap[i - firstOffset][1] == 'j');
            REQUIRE(snap[i - firstOffset][2] == 'j');
            REQUIRE(snap[i - firstOffset][3] == 'j');
            REQUIRE(snap[i - firstOffset][4] == 'j');
        }
        return snap;
    }

    void step1Claim1Page(void)
    {
        CHECK(writer.claim(item11, 1));
        item11[0] = 'I';
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
        Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
        CHECK(reader.retrieve(item11));
    }


    void step7Claim5Pages(void)
    {
        CHECK(writer.claim(item0, 5));
        item0[0] = 'j';
        item0[1] = 'j';
        item0[2] = 'j';
        item0[3] = 'j';
        item0[4] = 'j';
    }

    void step8Retrieve1Page(void)
    {
        CHECK(reader.retrieve(item9));
    }

    // The memory to use.
    unsigned char mem[sizeof(aq::CtrlOverlay) + 14 * sizeof(uint32_t) + 15];

    // The dummy trace manager.
    TraceManager m_tm;

    // The shared memory.
    AQExternMemory m_sm;

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
TEST_SUITE(UtUsageExample);

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

    REQUIRE(aq.item4[0] == 'D');

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

    REQUIRE(aq.item10[0] == 'H');

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

    REQUIRE(aq.item11[0] == 'I');

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

    REQUIRE(aq.item9[0] == 'g');
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

//------------------------------------------------------------------------------
TEST(given_UsageExampleCode_when_Executed_then_Runs)
{
    // EXAMPLE (1)
    AQHeapMemory mem(146);
    AQWriter writer(mem);
    AQReader reader(mem);
    reader.format(1, 500);
    cout << "Page Size        = " << reader.pageSize()  << endl;    // Page Size        = 2
    cout << "Page Count       = " << reader.pageCount() << endl;    // Page Count       = 15


    // Not included - setup the queue.
    UsageAQ uaq;
    uaq.initQueue(writer, reader);


    // EXAMPLE (2)
    AQSnapshot snap1(writer);
    cout << "Number of Items  = " << snap1.size() << endl;          // Number of Items  = 8
    cout << "Item[0] Size     = " << snap1[0].size() << endl;       // Item[0] Size     = 6
    cout << "Item[0][0]       = '" << snap1[0][0] << "'" << endl;   // Item[0][0]       = 'A'



    // EXAMPLE (3)
    AQWriterItem item1;
    writer.claim(item1, 2);
    item1[0] = 'I';
    item1[1] = 'I';


    // EXAMPLE (4)
    writer.commit(item1);


    // EXAMPLE (5)
    AQItem item2;
    reader.retrieve(item2);
    cout << "Item Size        = " << item2.size() << endl;          // Item Size        = 2
    cout << "Item[0]          = '" << item2[0] << "'" << endl;      // Item[0]          = 'D'


    // EXAMPLE (5)
    reader.release(item2);


    // EXAMPLE (6)
    AQItem item3;
    reader.retrieve(item3);
    cout << "Item Size        = " << item3.size() << endl;          // Item Size        = 2
    cout << "Item[0]          = '" << item3[0] << "'" << endl;      // Item[0]          = 'H'


    // EXAMPLE (7)
    Timer::sleep(AQTest::COMMIT_TIMEOUT_MS);
    AQItem item4;
    reader.retrieve(item4);
    cout << "Item Size        = " << item4.size() << endl;          // Item Size        = 2
    cout << "Item[0]          = '" << item4[0] << "'" << endl;      // Item[0]          = 'I'


    // EXAMPLE (8)
    AQWriterItem item5;
    writer.claim(item5, 10);
    item5[0] = 'J';


    // EXAMPLE (9)
    AQItem item6;
    reader.retrieve(item6);
    cout << "Item Size        = " << item6.size() << endl;          // Item Size        = 2
    cout << "Item[0]          = '" << item6[0] << "'" << endl;      // Item[0]          = 'G'
    cout << "Committed        = " << item6.isCommitted() << endl;   // Committed        = 0

    REQUIRE(reader.pageSize() == 2);
    REQUIRE(reader.pageCount() == 15);

    REQUIRE(snap1.size() == 8);
    REQUIRE(snap1[0].size() == 6);
    REQUIRE(snap1[0][0] == 'A');

}




//=============================== End of File ==================================
