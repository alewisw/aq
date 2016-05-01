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

// Used to create an extendable item in a known state for subsequent testing.
//
// The item structure is:
//   Item          #1 #2 #3 #4 #5  *REMAINDER*
//   Capacity:    | 4| 8| 4|12| 4|    2
//   Size:        | 4| 8| 4|12| 2    10
//   Accumulator: | 0| 4|12|16|28|
class ExtendableWitem
{
public:

    // Constructs the new extendable item.
    ExtendableWitem(AQTest& aq)
        : m_totalSize(0)
        , m_totalCapacity(0)
    {
        const size_t ps = 4;

        // We assume 4 byte pages for all these tests.
        CHECK(aq.pageSize() == 4);

        addMem(0xF0, 1 * ps);
        addMem(0xE0, 2 * ps);
        addMem(0xD0, 1 * ps);
        addMem(0xC0, 3 * ps);
        addMem(0xB0, 2);

        CHECK(aq.writer.claim(item, m_mem[0].size()));
        size_t i;
        for (i = 0; i < m_mem.size(); ++i)
        {
            CHECK(item.write(&m_mem[i][0], m_mem[i].size()));
        }
        const AQItem *it = NULL;
        for (i = 0, it = &item; i < m_mem.size() && it != NULL; ++i, it = it->next())
        {
            CHECK(it->size() == m_mem[i].size());
            CHECK(memcmp(&m_mem[i][0], &(*it)[0], m_mem[i].size()) == 0);

            m_totalSize += m_mem[i].size();
            m_totalCapacity += it->capacity();
        }
        CHECK(i == m_mem.size());
        CHECK(it == NULL);
    }

    // Destroys this item.
    ~ExtendableWitem(void)
    {
    }
private:

    // Adds an item to the memory list.  The item has values based on 'tag' an
    // is of length 'len'.
    void addMem(unsigned char tag, size_t len)
    {
        size_t idx = m_mem.size();
        m_mem.resize(idx + 1);
        for (size_t i = 0; i < len; ++i)
        {
            m_mem[idx].push_back((unsigned char)(tag | i));
        }
    }

    // The internal item memory list.
    vector< vector<unsigned char> > m_mem;

    // The total size and total capacity of all items.
    size_t m_totalSize;
    size_t m_totalCapacity;

public:

    // Returns true if and only if the content of this item is unchanged, ignoring
    // items at the listed indexes.
    bool isContentUnchanged(size_t ignoreIdx0 = SIZE_MAX)
    {
        bool changed = false;
        const AQItem *it = &item;
        for (size_t i = 0; i < m_mem.size() && it != NULL; ++i, it = it->next())
        {
            if (i != ignoreIdx0)
            {
                CHECK(it->size() >= m_mem[i].size());
                CHECK(memcmp(&m_mem[i][0], &(*it)[0], m_mem[i].size()) == 0);

                if (it->size() < m_mem[i].size() || memcmp(&m_mem[i][0], &(*it)[0], m_mem[i].size()) != 0)
                {
                    changed = true;
                }
            }
        }
        return !changed;
    }

    // The total initial capacity and size of the item.
    size_t initCapacity(void) const { return m_totalCapacity; }
    size_t initSize(void) const { return m_totalSize; }
    size_t initLastSize(void) const { return m_mem[m_mem.size() - 1].size(); }

    // The actual item.
    AQWriterItem item;
};



//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// A large (> 32 byte) string used for various tests.
static const char *const LargeString = "{abcdefghijklmnopqrstuvwxyz--ABCDEFGHIJKLMNOPQRSTUVWXYZ}";




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtWriterItem);

//------------------------------------------------------------------------------
TEST(given_WriterItemUnallocated_when_Write_then_DomainErrorException)
{
    AQWriterItem witem;
    char mem[5];

    REQUIRE_EXCEPTION(witem.write(mem, sizeof(mem)), domain_error);
}

//------------------------------------------------------------------------------
TEST(given_WriterItemUnallocated_when_WriteOffset_then_DomainErrorException)
{
    AQWriterItem witem;
    char mem[5];

    REQUIRE_EXCEPTION(witem.write(2, mem, sizeof(mem)), domain_error);
}

//------------------------------------------------------------------------------
TEST(given_WriterItemUnallocated_when_Printf_then_DomainErrorException)
{
    AQWriterItem witem;

    REQUIRE_EXCEPTION(witem.printf("%d", 2), domain_error);
}

//------------------------------------------------------------------------------
TEST(given_WriterItemUnallocated_when_PrintfOffset_then_DomainErrorException)
{
    AQWriterItem witem;

    REQUIRE_EXCEPTION(witem.printf(0, "%d", 2), domain_error);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemCommitted_when_Write_then_DomainErrorException)
{
    AQWriterItem witem;
    char mem[5];

    CHECK(aq.writer.claim(witem, 7));
    CHECK(aq.writer.commit(witem));

    REQUIRE_EXCEPTION(witem.write(mem, sizeof(mem)), domain_error);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemCommitted_when_WriteOffset_then_DomainErrorException)
{
    AQWriterItem witem;
    char mem[5];

    CHECK(aq.writer.claim(witem, 7));
    CHECK(aq.writer.commit(witem));

    REQUIRE_EXCEPTION(witem.write(2, mem, sizeof(mem)), domain_error);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemCommitted_when_Printf_then_DomainErrorException)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    CHECK(aq.writer.commit(witem));

    REQUIRE_EXCEPTION(witem.printf("%d", 2), domain_error);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemCommitted_when_PrintfOffset_then_DomainErrorException)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    CHECK(aq.writer.commit(witem));

    REQUIRE_EXCEPTION(witem.printf(0, "%d", 2), domain_error);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteMemNull_then_InvalidArgumentException)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));

    REQUIRE_EXCEPTION(witem.write(NULL, 5), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteOffsetMemNull_then_InvalidArgumentException)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));

    REQUIRE_EXCEPTION(witem.write(2, NULL, 5), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfFmtNull_then_InvalidArgumentException)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));

    REQUIRE_EXCEPTION(witem.printf(NULL, 5), invalid_argument);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffsetFmtNull_then_InvalidArgumentException)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));

    REQUIRE_EXCEPTION(witem.printf(2, NULL, 5), invalid_argument);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemClaimed_when_WriteOffsetAtSize_then_ZeroBytesWritten(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(0 == witem.write(sizeof(mem), mem, 1, action));
}
AQTEST(given_WriterItemClaimed_when_WriteOffsetAtSize_then_ZeroBytesWritten)
{
    DO_given_WriterItemClaimed_when_WriteOffsetAtSize_then_ZeroBytesWritten(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemClaimed_when_WritePartialOffsetAtSize_then_ZeroBytesWritten)
{
    DO_given_WriterItemClaimed_when_WriteOffsetAtSize_then_ZeroBytesWritten(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemWrittenToSize_when_Write1Byte_then_ZeroBytesWritten(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    CHECK(sizeof(mem) == witem.write(mem, sizeof(mem), action));
    REQUIRE(0 == witem.write(mem, 1));
}
AQTEST(given_WriterItemWrittenToSize_when_Write1Byte_then_ZeroBytesWritten)
{
    DO_given_WriterItemWrittenToSize_when_Write1Byte_then_ZeroBytesWritten(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemWrittenToSize_when_WritePartial1Byte_then_ZeroBytesWritten)
{
    DO_given_WriterItemWrittenToSize_when_Write1Byte_then_ZeroBytesWritten(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteMemNullLength0_then_NoActionTaken)
{
    AQWriterItem witem;
    char mem[] = { 9,8,7 };

    CHECK(aq.writer.claim(witem, 7));

    memset(&witem[0], 0xCD, sizeof(mem));

    REQUIRE(witem.write(NULL, 0) == 0);
    CHECK(sizeof(mem) == witem.write(mem, sizeof(mem)));

    REQUIRE(witem[0] == 9);
    REQUIRE(witem[1] == 8);
    REQUIRE(witem[2] == 7);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemClaimed_when_WriteOffsetMemNullLength0_then_PositionMoved(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5 };

    CHECK(aq.writer.claim(witem, 7));

    witem[0] = 0x10;
    witem[1] = 0x20;
    memset(&witem[2], 0xCD, sizeof(mem));

    REQUIRE(witem.write(2, NULL, 0) == 0);
    CHECK(sizeof(mem) == witem.write(mem, sizeof(mem), action));

    REQUIRE(witem[0] == 0x10);
    REQUIRE(witem[1] == 0x20);
    REQUIRE(witem[2] == 0x01);
    REQUIRE(witem[3] == 0x02);
    REQUIRE(witem[4] == 0x03);
    REQUIRE(witem[5] == 0x04);
    REQUIRE(witem[6] == 0x05);
}
AQTEST(given_WriterItemClaimed_when_WriteOffsetMemNullLength0_then_PositionMoved)
{
    DO_given_WriterItemClaimed_when_WriteOffsetMemNullLength0_then_PositionMoved(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemClaimed_when_WritePartialOffsetMemNullLength0_then_PositionMoved)
{
    DO_given_WriterItemClaimed_when_WriteOffsetMemNullLength0_then_PositionMoved(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemClaimed_when_WriteItemSize_then_WriteSucceeds(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(sizeof(mem) == witem.write(mem, sizeof(mem), action));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}
AQTEST(given_WriterItemClaimed_when_WriteItemSize_then_WriteSucceeds)
{
    DO_given_WriterItemClaimed_when_WriteItemSize_then_WriteSucceeds(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemClaimed_when_WritePartialItemSize_then_WriteSucceeds)
{
    DO_given_WriterItemClaimed_when_WriteItemSize_then_WriteSucceeds(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemClaimed_when_WriteItemSizeInMultipleSteps_then_WriteSucceeds(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(4 == witem.write(&mem[0], 4, action));
    REQUIRE(3 == witem.write(&mem[4], 3, action));
    REQUIRE(1 == witem.write(&mem[7], 1, action));
    REQUIRE(6 == witem.write(&mem[8], 6, action));
    REQUIRE(sizeof(mem) - 14 == witem.write(&mem[14], sizeof(mem) - 14, action));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}
AQTEST(given_WriterItemClaimed_when_WriteItemSizeInMultipleSteps_then_WriteSucceeds)
{
    DO_given_WriterItemClaimed_when_WriteItemSizeInMultipleSteps_then_WriteSucceeds(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemClaimed_when_WritePartialItemSizeInMultipleSteps_then_WriteSucceeds)
{
    DO_given_WriterItemClaimed_when_WriteItemSizeInMultipleSteps_then_WriteSucceeds(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteItemSizePlus1_then_ZeroBytesWritten)
{
    AQWriterItem witem;
    char mema[] = { 1, 2, 3, 4, 5 };
    char memb[] = { 9, 8, 7, 6, 0xA, 0xB };

    CHECK(aq.writer.claim(witem, sizeof(mema)));
    memcpy(&witem[0], mema, sizeof(mema));
    REQUIRE(witem.write(memb, sizeof(memb)) == 0);
    REQUIRE(memcmp(mema, &witem[0], sizeof(mema)) == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WritePartialItemSizePlus1_then_ItemSizeBytesWritten)
{
    AQWriterItem witem;
    char mema[] = { 1, 2, 3, 4, 5 };
    char memb[] = { 9, 8, 7, 6, 0xA, 0xB };

    CHECK(aq.writer.claim(witem, sizeof(mema)));
    memcpy(&witem[0], mema, sizeof(mema));
    REQUIRE(witem.write(memb, sizeof(memb), AQWriterItem::WRITE_PARTIAL) == sizeof(mema));
    REQUIRE(memcmp(memb, &witem[0], sizeof(mema)) == 0);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemClaimed_when_WriteWithOffsetItemSizeInMultipleSteps_then_WriteSucceeds(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(1 == witem.write(7, &mem[7], 1, action));
    REQUIRE(4 == witem.write(0, &mem[0], 4, action));
    REQUIRE(sizeof(mem) - 14 == witem.write(14, &mem[14], sizeof(mem) - 14, action));
    REQUIRE(6 == witem.write(8, &mem[8], 6, action));
    REQUIRE(3 == witem.write(4, &mem[4], 3, action));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}
AQTEST(given_WriterItemClaimed_when_WriteWithOffsetItemSizeInMultipleSteps_then_WriteSucceeds)
{
    DO_given_WriterItemClaimed_when_WriteWithOffsetItemSizeInMultipleSteps_then_WriteSucceeds(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemClaimed_when_WritePartialWithOffsetItemSizeInMultipleSteps_then_WriteSucceeds)
{
    DO_given_WriterItemClaimed_when_WriteWithOffsetItemSizeInMultipleSteps_then_WriteSucceeds(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemClaimed_when_WriteWithOffset_then_WritePositionBecomesNextByte(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(1 == witem.write(7, &mem[7], 1, action));
    REQUIRE(6 == witem.write(&mem[8], 6, action));

    REQUIRE(4 == witem.write(0, &mem[0], 4, action));
    REQUIRE(sizeof(mem) - 14 == witem.write(&mem[14], sizeof(mem) - 14, action));

    REQUIRE(3 == witem.write(4, &mem[4], 3, action));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}
AQTEST(given_WriterItemClaimed_when_WriteWithOffset_then_WritePositionBecomesNextByte)
{
    DO_given_WriterItemClaimed_when_WriteWithOffset_then_WritePositionBecomesNextByte(aq, AQWriterItem::WRITE_NONE);
}
AQTEST(given_WriterItemClaimed_when_WritePartialWithOffset_then_WritePositionBecomesNextByte)
{
    DO_given_WriterItemClaimed_when_WriteWithOffset_then_WritePositionBecomesNextByte(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacity_then_ItemExtended(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    REQUIRE(sizeof(mem) == w.item.write(w.initCapacity() + 3, mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged());

    const AQItem& last = *w.item.last();
    REQUIRE(last.prev()->size() == last.prev()->capacity());
    REQUIRE(last.size() == 3 + sizeof(mem));
    REQUIRE(memcmp(&last[3], mem, sizeof(mem)) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacity_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacity_then_ItemExtended(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithOffsetLargerThanCapacity_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacity_then_ItemExtended(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromMiddleItem_then_ItemExtended(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    AQWriterItem *n1 = w.item.next();
    AQWriterItem *n2 = n1->next();

    REQUIRE(n2->write(w.initCapacity() + 3 - n1->capacity() - w.item.capacity(), mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged());

    const AQItem& last = *w.item.last();
    REQUIRE(last.prev()->size() == last.prev()->capacity());
    REQUIRE(last.size() == 3 + sizeof(mem));
    REQUIRE(memcmp(&last[3], mem, sizeof(mem)) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromMiddleItem_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromMiddleItem_then_ItemExtended(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithOffsetLargerThanCapacityFromMiddleItem_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromMiddleItem_then_ItemExtended(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromLastItem_then_ItemExtended(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    AQWriterItem& tgt = *w.item.last();
    REQUIRE(sizeof(mem) == tgt.write(tgt.capacity() + 3, mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged());

    const AQItem& last = *w.item.last();
    REQUIRE(last.prev()->size() == last.prev()->capacity());
    REQUIRE(last.size() == 3 + sizeof(mem));
    REQUIRE(memcmp(&last[3], mem, sizeof(mem)) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromLastItem_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromLastItem_then_ItemExtended(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithOffsetLargerThanCapacityFromLastItem_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromLastItem_then_ItemExtended(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingBounds_then_ItemSizeUnchanged(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    REQUIRE(sizeof(mem) == w.item.write(w.item.capacity(), mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged(1));

    const AQItem& cmp = *w.item.next();
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(memcmp(&cmp[0], mem, sizeof(mem)) == 0);
    REQUIRE(cmp.last()->size() == w.initLastSize());
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithAlignedOffsetWithinExistingBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingBounds_then_ItemSizeUnchanged(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    REQUIRE(sizeof(mem) == w.item.write(w.item.capacity() + 4, mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged(1));

    const AQItem& cmp = *w.item.next();
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(memcmp(&cmp[4], mem, sizeof(mem)) == 0);
    REQUIRE(cmp.last()->size() == w.initLastSize());
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithUnalignedOffsetWithinExistingBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(sizeof(mem) == w.item.write(w.initSize() - cmp.size(), mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged(4));

    REQUIRE(cmp.size() == w.initLastSize());
    REQUIRE(memcmp(&cmp[0], mem, sizeof(mem)) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithAlignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 2 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(sizeof(mem) == w.item.write(w.initSize() + 1 - cmp.size(), mem, sizeof(mem), action));
    REQUIRE(w.isContentUnchanged(4));

    REQUIRE(cmp.size() == w.initLastSize());
    REQUIRE(memcmp(&cmp[1], mem, sizeof(mem)) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialWithUnalignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteExtendsSize_then_WritePositionIncreased(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    const AQItem *cmp = w.item.last();
    REQUIRE(3 == w.item.write(&mem[0], 3, action));
    REQUIRE(2 == w.item.write(&mem[3], 2, action));
    REQUIRE(4 == w.item.write(&mem[5], 4, action));
    REQUIRE(w.isContentUnchanged());

    REQUIRE(cmp->size() == cmp->capacity());
    REQUIRE(memcmp(&(*cmp)[2], &mem[0], 2) == 0);

    cmp = cmp->next();
    REQUIRE(cmp != NULL);
    REQUIRE(cmp->size() == 4);
    REQUIRE(cmp->capacity() == 4);
    REQUIRE(memcmp(&(*cmp)[0], &mem[2], 4) == 0);

    cmp = cmp->next();
    REQUIRE(cmp != NULL);
    REQUIRE(cmp->size() == 3);
    REQUIRE(cmp->capacity() == 4);
    REQUIRE(memcmp(&(*cmp)[0], &mem[6], 3) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteExtendsSize_then_WritePositionIncreased(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteExtendsSize_then_WritePositionIncreased(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteBeforeEndExtendsSize_then_WritePositionIncreased(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    const AQItem *cmp = w.item.last();
    REQUIRE(3 == w.item.write(w.initSize() - 1, &mem[0], 3, action));
    REQUIRE(2 == w.item.write(&mem[3], 2, action));
    REQUIRE(4 == w.item.write(&mem[5], 4, action));
    REQUIRE(w.isContentUnchanged(4));

    REQUIRE(cmp->size() == cmp->capacity());
    REQUIRE(memcmp(&(*cmp)[1], &mem[0], 3) == 0);

    cmp = cmp->next();
    REQUIRE(cmp != NULL);
    REQUIRE(cmp->size() == 4);
    REQUIRE(cmp->capacity() == 4);
    REQUIRE(memcmp(&(*cmp)[0], &mem[3], 4) == 0);

    cmp = cmp->next();
    REQUIRE(cmp != NULL);
    REQUIRE(cmp->size() == 2);
    REQUIRE(cmp->capacity() == 4);
    REQUIRE(memcmp(&(*cmp)[0], &mem[7], 2) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteBeforeEndExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteBeforeEndExtendsSize_then_WritePositionIncreased(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialBeforeEndExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteBeforeEndExtendsSize_then_WritePositionIncreased(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
static void DO_given_WriterItemExtendable_when_WriteAfterEndExtendsSize_then_WritePositionIncreased(AQTest& aq, AQWriterItem::InsufficientSpaceAction action)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    const AQItem *cmp = w.item.last();
    REQUIRE(3 == w.item.write(w.initSize() + 1, &mem[0], 3, action));
    REQUIRE(2 == w.item.write(&mem[3], 2, action));
    REQUIRE(4 == w.item.write(&mem[5], 4, action));
    REQUIRE(w.isContentUnchanged());

    REQUIRE(cmp->size() == cmp->capacity());
    REQUIRE(memcmp(&(*cmp)[3], &mem[0], 1) == 0);

    cmp = cmp->next();
    REQUIRE(cmp != NULL);
    REQUIRE(cmp->size() == 4);
    REQUIRE(cmp->capacity() == 4);
    REQUIRE(memcmp(&(*cmp)[0], &mem[1], 4) == 0);

    cmp = cmp->next();
    REQUIRE(cmp != NULL);
    REQUIRE(cmp->size() == 4);
    REQUIRE(cmp->capacity() == 4);
    REQUIRE(memcmp(&(*cmp)[0], &mem[5], 4) == 0);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteAfterEndExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteAfterEndExtendsSize_then_WritePositionIncreased(aq, AQWriterItem::WRITE_NONE);
}
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialAfterEndExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    DO_given_WriterItemExtendable_when_WriteAfterEndExtendsSize_then_WritePositionIncreased(aq, AQWriterItem::WRITE_PARTIAL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteQueueAvailableSizePlus1_then_NoDataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(0 == w.item.write(mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged());
    REQUIRE(&cmp == w.item.last());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WritePartialQueueAvailableSizePlus1_then_QueueFilledAndExistingItemWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(10 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(cmp[2] == 1);
    REQUIRE(cmp[3] == 2);
    REQUIRE(cmp.next() != NULL);
    const AQItem& last = *cmp.next();
    REQUIRE(last.size() == 8);
    REQUIRE(last[0] == 3);
    REQUIRE(last[1] == 4);
    REQUIRE(last[2] == 5);
    REQUIRE(last[3] == 6);
    REQUIRE(last[4] == 7);
    REQUIRE(last[5] == 8);
    REQUIRE(last[6] == 9);
    REQUIRE(last[7] == 10);
    REQUIRE(last.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_Queue1PageFreeWriterItemExtendable_when_WritePartialOffsetAtEndOfQueue_then_LastItemAllocated, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6 };

    AQWriterItem& cmp = *w.item.last();
    REQUIRE(6 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(cmp[2] == 1);
    REQUIRE(cmp[3] == 2);
    REQUIRE(cmp.next() != NULL);
    AQWriterItem& last = *cmp.next();
    REQUIRE(last.size() == 4);
    REQUIRE(last[0] == 3);
    REQUIRE(last[1] == 4);
    REQUIRE(last[2] == 5);
    REQUIRE(last[3] == 6);
    REQUIRE(last.next() == NULL);

    REQUIRE(0 == last.write(4, mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(last.next() != NULL);
    REQUIRE(last.next()->size() == aq.pageSize());
    REQUIRE(last.next()->capacity() == aq.pageSize());
    REQUIRE(last.next()->next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_Queue1PageFreeWriterItemExtendable_when_WritePartialOffsetBeyondEndOfQueue_then_LastItemAllocated, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6 };

    AQWriterItem& cmp = *w.item.last();
    REQUIRE(6 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(cmp[2] == 1);
    REQUIRE(cmp[3] == 2);
    REQUIRE(cmp.next() != NULL);
    AQWriterItem& last = *cmp.next();
    REQUIRE(last.size() == 4);
    REQUIRE(last[0] == 3);
    REQUIRE(last[1] == 4);
    REQUIRE(last[2] == 5);
    REQUIRE(last[3] == 6);
    REQUIRE(last.next() == NULL);

    REQUIRE(0 == last.write(49999, mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(last.next() != NULL);
    REQUIRE(last.next()->size() == aq.pageSize());
    REQUIRE(last.next()->capacity() == aq.pageSize());
    REQUIRE(last.next()->next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_QueueFullWriterItemExtendable_when_WritePartialExtendsQueue_then_ItemWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(8 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(2 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(cmp[2] == 1);
    REQUIRE(cmp[3] == 2);
    REQUIRE(cmp.next() != NULL);
    const AQItem& last = *cmp.next();
    REQUIRE(last.size() == 8);
    REQUIRE(last[0] == 3);
    REQUIRE(last[1] == 4);
    REQUIRE(last[2] == 5);
    REQUIRE(last[3] == 6);
    REQUIRE(last[4] == 7);
    REQUIRE(last[5] == 8);
    REQUIRE(last[6] == 1);
    REQUIRE(last[7] == 2);
    REQUIRE(last.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_QueueFullWriterItemExtendable_when_WritePartialOffsetAtEndOfQueue_then_LastItemSizeIncreased, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    AQWriterItem& cmp = *w.item.last();
    REQUIRE(8 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(cmp[2] == 1);
    REQUIRE(cmp[3] == 2);
    REQUIRE(cmp.next() != NULL);
    AQWriterItem& last = *cmp.next();
    REQUIRE(last.size() == 6);
    REQUIRE(last[0] == 3);
    REQUIRE(last[1] == 4);
    REQUIRE(last[2] == 5);
    REQUIRE(last[3] == 6);
    REQUIRE(last[4] == 7);
    REQUIRE(last[5] == 8);
    REQUIRE(last.next() == NULL);

    REQUIRE(0 == last.write(8, mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(last.size() == 8);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_QueueFullWriterItemExtendable_when_WritePartialOffsetAfterEndOfQueue_then_LastItemSizeIncreased, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    AQWriterItem& cmp = *w.item.last();
    REQUIRE(8 == w.item.write(mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(cmp[2] == 1);
    REQUIRE(cmp[3] == 2);
    REQUIRE(cmp.next() != NULL);
    AQWriterItem& last = *cmp.next();
    REQUIRE(last.size() == 6);
    REQUIRE(last[0] == 3);
    REQUIRE(last[1] == 4);
    REQUIRE(last[2] == 5);
    REQUIRE(last[3] == 6);
    REQUIRE(last[4] == 7);
    REQUIRE(last[5] == 8);
    REQUIRE(last.next() == NULL);

    REQUIRE(0 == last.write(999, mem, sizeof(mem), AQWriterItem::WRITE_PARTIAL));
    REQUIRE(last.size() == 8);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfZeroBytes_then_Returns0)
{
    unsigned char mem[] = { 1, 2, 3, 4 };

    AQWriterItem witem;
    CHECK(aq.writer.claim(witem, sizeof(mem)));
    CHECK(sizeof(mem) == witem.write(mem, sizeof(mem)));
    REQUIRE(0 == witem.printf("%s", ""));
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_Printf1Byte_then_ReturnsFailure)
{
    unsigned char mem[] = { 1, 2, 3, 4 };

    AQWriterItem witem;
    CHECK(aq.writer.claim(witem, sizeof(mem)));
    CHECK(sizeof(mem) == witem.write(mem, sizeof(mem)));
    REQUIRE(~0 == witem.printf("%s", "f"));
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfLessThanSize_then_DataValid)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(4 == witem.printf("%d", 5491));
    REQUIRE(witem[0] == '5');
    REQUIRE(witem[1] == '4');
    REQUIRE(witem[2] == '9');
    REQUIRE(witem[3] == '1');
    REQUIRE(witem[4] == 0xDD);
    REQUIRE(witem[5] == 0xDD);
    REQUIRE(witem[6] == 0xDD);
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffsetLessThanSize_then_DataValid)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(4 == witem.printf(2, "%d", 5491));
    REQUIRE(witem[0] == 0xDD);
    REQUIRE(witem[1] == 0xDD);
    REQUIRE(witem[2] == '5');
    REQUIRE(witem[3] == '4');
    REQUIRE(witem[4] == '9');
    REQUIRE(witem[5] == '1');
    REQUIRE(witem[6] == 0xDD);
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfEqualsSize_then_DataValid)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(7 == witem.printf("%d", 5491358));
    REQUIRE(witem[0] == '5');
    REQUIRE(witem[1] == '4');
    REQUIRE(witem[2] == '9');
    REQUIRE(witem[3] == '1');
    REQUIRE(witem[4] == '3');
    REQUIRE(witem[5] == '5');
    REQUIRE(witem[6] == '8');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffsetEqualsSize_then_DataValid)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(4 == witem.printf(3, "%d", 1358));
    REQUIRE(witem[0] == 0xDD);
    REQUIRE(witem[1] == 0xDD);
    REQUIRE(witem[2] == 0xDD);
    REQUIRE(witem[3] == '1');
    REQUIRE(witem[4] == '3');
    REQUIRE(witem[5] == '5');
    REQUIRE(witem[6] == '8');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfEqualsSizePlus1_then_DataValid)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(~7 == witem.printf("%d", 54913581));
    REQUIRE(witem[0] == '5');
    REQUIRE(witem[1] == '4');
    REQUIRE(witem[2] == '9');
    REQUIRE(witem[3] == '1');
    REQUIRE(witem[4] == '3');
    REQUIRE(witem[5] == '5');
    REQUIRE(witem[6] == '8');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffsetEqualsSizePlus1_then_DataValid)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(~4 == witem.printf(3, "%d", 13589));
    REQUIRE(witem[0] == 0xDD);
    REQUIRE(witem[1] == 0xDD);
    REQUIRE(witem[2] == 0xDD);
    REQUIRE(witem[3] == '1');
    REQUIRE(witem[4] == '3');
    REQUIRE(witem[5] == '5');
    REQUIRE(witem[6] == '8');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfGreaterThanSize_then_DataTruncated)
{
    AQWriterItem witem;
    
    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(~7 == witem.printf("%d %s%s%s%s%s%s%s%s", 54913589, LargeString, LargeString, LargeString, LargeString, LargeString, LargeString, LargeString, LargeString));
    REQUIRE(witem[0] == '5');
    REQUIRE(witem[1] == '4');
    REQUIRE(witem[2] == '9');
    REQUIRE(witem[3] == '1');
    REQUIRE(witem[4] == '3');
    REQUIRE(witem[5] == '5');
    REQUIRE(witem[6] == '8');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffsetGreaterThanSize_then_DataTruncated)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(~3 == witem.printf(4, "%d %s%s%s%s%s%s%s%s", 54913589, LargeString, LargeString, LargeString, LargeString, LargeString, LargeString, LargeString, LargeString));
    REQUIRE(witem[0] == 0xDD);
    REQUIRE(witem[1] == 0xDD);
    REQUIRE(witem[2] == 0xDD);
    REQUIRE(witem[3] == 0xDD);
    REQUIRE(witem[4] == '5');
    REQUIRE(witem[5] == '4');
    REQUIRE(witem[6] == '9');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfInSequence_then_EachPrintAppends)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(2 == witem.printf("%d", 12));
    REQUIRE(3 == witem.printf("%d", 345));
    REQUIRE(1 == witem.printf("%d", 6));
    REQUIRE(1 == witem.printf("%d", 7));
    REQUIRE(witem[0] == '1');
    REQUIRE(witem[1] == '2');
    REQUIRE(witem[2] == '3');
    REQUIRE(witem[3] == '4');
    REQUIRE(witem[4] == '5');
    REQUIRE(witem[5] == '6');
    REQUIRE(witem[6] == '7');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffset_then_WritePositionChanged)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(2 == witem.printf(3, "%d", 45));
    REQUIRE(2 == witem.printf("%d", 67));
    REQUIRE(witem[0] == 0xDD);
    REQUIRE(witem[1] == 0xDD);
    REQUIRE(witem[2] == 0xDD);
    REQUIRE(witem[3] == '4');
    REQUIRE(witem[4] == '5');
    REQUIRE(witem[5] == '6');
    REQUIRE(witem[6] == '7');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_PrintfOffsetOutOfSequence_then_EachPrintSucceeds)
{
    AQWriterItem witem;

    CHECK(aq.writer.claim(witem, 7));
    memset(&witem[0], 0xDD, 8);
    REQUIRE(1 == witem.printf(6, "%d", 7));
    REQUIRE(3 == witem.printf(2, "%d", 345));
    REQUIRE(1 == witem.printf(5, "%d", 6));
    REQUIRE(2 == witem.printf(0, "%d", 12));
    REQUIRE(witem[0] == '1');
    REQUIRE(witem[1] == '2');
    REQUIRE(witem[2] == '3');
    REQUIRE(witem[3] == '4');
    REQUIRE(witem[4] == '5');
    REQUIRE(witem[5] == '6');
    REQUIRE(witem[6] == '7');
    REQUIRE(witem[7] == 0xDD);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfLessThanCapacity_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(1 == w.item.printf("%d", 9));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == 0xDD);
    REQUIRE(cmp.size() == 3);
    REQUIRE(cmp.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfEqualsCapacity_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(2 == w.item.printf("%d", 92));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfEqualsCapacityPlus1_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(3 == w.item.printf("%d", 928));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() != NULL);
    const AQItem& next = *cmp.next();
    REQUIRE(next[0] == '8');
    REQUIRE(next.size() == 1);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfEquals1ExtraPage_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(6 == w.item.printf("%dFah", 928));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() != NULL);
    const AQItem& next = *cmp.next();
    REQUIRE(next[0] == '8');
    REQUIRE(next[1] == 'F');
    REQUIRE(next[2] == 'a');
    REQUIRE(next[3] == 'h');
    REQUIRE(next.size() == 4);
    REQUIRE(next.capacity() == 4);
    REQUIRE(next.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfEquals2ExtraPages_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(7 == w.item.printf("%dFah%c", 928, 'j'));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() != NULL);
    const AQItem& next = *cmp.next();
    REQUIRE(next[0] == '8');
    REQUIRE(next[1] == 'F');
    REQUIRE(next[2] == 'a');
    REQUIRE(next[3] == 'h');
    REQUIRE(next[4] == 'j');
    REQUIRE(next.size() == 5);
    REQUIRE(next.capacity() == 8);
    REQUIRE(next.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfEqualsRemainingSpace_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(10 == w.item.printf("%dFah%c789", 928, 'j'));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() != NULL);
    const AQItem& next = *cmp.next();
    REQUIRE(next[0] == '8');
    REQUIRE(next[1] == 'F');
    REQUIRE(next[2] == 'a');
    REQUIRE(next[3] == 'h');
    REQUIRE(next[4] == 'j');
    REQUIRE(next[5] == '7');
    REQUIRE(next[6] == '8');
    REQUIRE(next[7] == '9');
    REQUIRE(next.size() == 8);
    REQUIRE(next.capacity() == 8);
    REQUIRE(next.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfEqualsRemainingSpacePlus1_then_DataTruncated, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(~10 == w.item.printf("%dFah%c789l", 928, 'j'));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() != NULL);
    const AQItem& next = *cmp.next();
    REQUIRE(next[0] == '8');
    REQUIRE(next[1] == 'F');
    REQUIRE(next[2] == 'a');
    REQUIRE(next[3] == 'h');
    REQUIRE(next[4] == 'j');
    REQUIRE(next[5] == '7');
    REQUIRE(next[6] == '8');
    REQUIRE(next[7] == '9');
    REQUIRE(next.size() == 8);
    REQUIRE(next.capacity() == 8);
    REQUIRE(next.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfInSequence_then_DataWritten, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    memset(&cmp[0], 0xDD, cmp.capacity());
    REQUIRE(3 == w.item.printf("%d", 928));
    REQUIRE(1 == w.item.printf("F"));
    REQUIRE(3 == w.item.printf("ah%c", 'j'));
    REQUIRE(3 == w.item.printf("789"));
    REQUIRE(w.isContentUnchanged(4));
    REQUIRE(cmp[0] == 0xDD);
    REQUIRE(cmp[1] == 0xDD);
    REQUIRE(cmp[2] == '9');
    REQUIRE(cmp[3] == '2');
    REQUIRE(cmp.size() == 4);
    REQUIRE(cmp.next() != NULL);
    const AQItem& next = *cmp.next();
    REQUIRE(next[0] == '8');
    REQUIRE(next[1] == 'F');
    REQUIRE(next[2] == 'a');
    REQUIRE(next[3] == 'h');
    REQUIRE(next.size() == 4);
    REQUIRE(next.capacity() == 4);
    const AQItem& last = *next.next();
    REQUIRE(last[0] == 'j');
    REQUIRE(last[1] == '7');
    REQUIRE(last[2] == '8');
    REQUIRE(last[3] == '9');
    REQUIRE(last.size() == 4);
    REQUIRE(last.capacity() == 4);
    REQUIRE(last.next() == NULL);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfOffsetPastEnd_then_QueueExtended, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    REQUIRE(~0 == cmp.printf(12, "1"));
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_PrintfOffsetPastEndPlus1_then_QueueExtended, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);

    AQWriterItem& cmp = *w.item.last();
    REQUIRE(~0 == cmp.printf(13, "1"));
}




//=============================== End of File ==================================
