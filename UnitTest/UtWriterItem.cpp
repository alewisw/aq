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

// Used to create an extendable item in a known state for subsequent testing.
//
// The item structure is:
//   Item          #1 #2 #3 #4 #5
//   Capacity:    | 4| 8| 4|12| 4|
//   Size:        | 4| 8| 4|12| 2
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
            m_mem[idx].push_back(tag | i);
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




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtWriterItem);

//------------------------------------------------------------------------------
AQTEST(given_WriterItemUnallocated_when_Write_then_DomainErrorException)
{
    AQWriterItem witem;
    char mem[5];

    REQUIRE_EXCEPTION(witem.write(mem, sizeof(mem)), domain_error);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemUnallocated_when_WriteOffset_then_DomainErrorException)
{
    AQWriterItem witem;
    char mem[5];

    REQUIRE_EXCEPTION(witem.write(2, mem, sizeof(mem)), domain_error);
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
AQTEST(given_WriterItemClaimed_when_WriteOffsetAtSize_then_OutOfRangeException)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE_EXCEPTION(witem.write(sizeof(mem), mem, 1), out_of_range);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemWrittenToSize_when_Write1Byte_then_OutOfRangeException)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    CHECK(witem.write(mem, sizeof(mem)));
    REQUIRE_EXCEPTION(witem.write(mem, 1), out_of_range);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteMemNullLength0_then_NoActionTaken)
{
    AQWriterItem witem;
    char mem[] = { 9,8,7 };

    CHECK(aq.writer.claim(witem, 7));

    memset(&witem[0], 0xCD, sizeof(mem));

    REQUIRE(witem.write(NULL, 0));
    CHECK(witem.write(mem, sizeof(mem)));

    REQUIRE(witem[0] == 9);
    REQUIRE(witem[1] == 8);
    REQUIRE(witem[2] == 7);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteOffsetMemNullLength0_then_PositionMoved)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5 };

    CHECK(aq.writer.claim(witem, 7));

    witem[0] = 0x10;
    witem[1] = 0x20;
    memset(&witem[2], 0xCD, sizeof(mem));

    REQUIRE(witem.write(2, NULL, 0));
    CHECK(witem.write(mem, sizeof(mem)));

    REQUIRE(witem[0] == 0x10);
    REQUIRE(witem[1] == 0x20);
    REQUIRE(witem[2] == 0x01);
    REQUIRE(witem[3] == 0x02);
    REQUIRE(witem[4] == 0x03);
    REQUIRE(witem[5] == 0x04);
    REQUIRE(witem[6] == 0x05);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteItemSize_then_WriteSucceeds)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(witem.write(mem, sizeof(mem)));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteItemSizeInMultipleSteps_then_WriteSucceeds)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(witem.write(&mem[0], 4));
    REQUIRE(witem.write(&mem[4], 3));
    REQUIRE(witem.write(&mem[7], 1));
    REQUIRE(witem.write(&mem[8], 6));
    REQUIRE(witem.write(&mem[14], sizeof(mem) - 14));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteItemSizePlus1_then_LengthErrorException)
{
    AQWriterItem witem;
    char mema[] = { 1, 2, 3, 4, 5 };
    char memb[] = { 9, 8, 7, 6, 0xA, 0xB };

    CHECK(aq.writer.claim(witem, sizeof(mema)));
    memcpy(&witem[0], mema, sizeof(mema));
    REQUIRE_EXCEPTION(witem.write(memb, sizeof(memb)), length_error);
    REQUIRE(memcmp(mema, &witem[0], sizeof(mema)) == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteWithOffsetItemSizeInMultipleSteps_then_WriteSucceeds)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(witem.write(7, &mem[7], 1));
    REQUIRE(witem.write(0, &mem[0], 4));
    REQUIRE(witem.write(14, &mem[14], sizeof(mem) - 14));
    REQUIRE(witem.write(8, &mem[8], 6));
    REQUIRE(witem.write(4, &mem[4], 3));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_WriterItemClaimed_when_WriteWithOffset_then_WritePositionBecomesNextByte)
{
    AQWriterItem witem;
    char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    CHECK(aq.writer.claim(witem, sizeof(mem)));
    REQUIRE(witem.write(7, &mem[7], 1));
    REQUIRE(witem.write(&mem[8], 6));

    REQUIRE(witem.write(0, &mem[0], 4));
    REQUIRE(witem.write(&mem[14], sizeof(mem) - 14));

    REQUIRE(witem.write(4, &mem[4], 3));
    REQUIRE(memcmp(mem, &witem[0], sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacity_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    REQUIRE(w.item.write(w.initCapacity() + 3, mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged());

    const AQItem& last = *w.item.last();
    REQUIRE(last.prev()->size() == last.prev()->capacity());
    REQUIRE(last.size() == 3 + sizeof(mem));
    REQUIRE(memcmp(&last[3], mem, sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromMiddleItem_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    AQWriterItem *n1 = w.item.next();
    AQWriterItem *n2 = n1->next();

    REQUIRE(n2->write(w.initCapacity() + 3 - n1->capacity() - w.item.capacity(), mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged());

    const AQItem& last = *w.item.last();
    REQUIRE(last.prev()->size() == last.prev()->capacity());
    REQUIRE(last.size() == 3 + sizeof(mem));
    REQUIRE(memcmp(&last[3], mem, sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithOffsetLargerThanCapacityFromLastItem_then_ItemExtended, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    AQWriterItem *n1 = w.item.next();
    AQWriterItem *n2 = n1->next();

    AQWriterItem& tgt = *w.item.last();
    REQUIRE(tgt.write(tgt.capacity() + 3, mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged());

    const AQItem& last = *w.item.last();
    REQUIRE(last.prev()->size() == last.prev()->capacity());
    REQUIRE(last.size() == 3 + sizeof(mem));
    REQUIRE(memcmp(&last[3], mem, sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    REQUIRE(w.item.write(w.item.capacity(), mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged(1));

    const AQItem& cmp = *w.item.next();
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(memcmp(&cmp[0], mem, sizeof(mem)) == 0);
    REQUIRE(cmp.last()->size() == w.initLastSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4 };

    REQUIRE(w.item.write(w.item.capacity() + 4, mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged(1));

    const AQItem& cmp = *w.item.next();
    REQUIRE(cmp.size() == cmp.capacity());
    REQUIRE(memcmp(&cmp[4], mem, sizeof(mem)) == 0);
    REQUIRE(cmp.last()->size() == w.initLastSize());
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithAlignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(w.item.write(w.initSize() - cmp.size(), mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged(4));

    REQUIRE(cmp.size() == w.initLastSize());
    REQUIRE(memcmp(&cmp[0], mem, sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteWithUnalignedOffsetWithinExistingEndBounds_then_ItemSizeUnchanged, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 2 };

    const AQItem& cmp = *w.item.last();
    REQUIRE(w.item.write(w.initSize() + 1 - cmp.size(), mem, sizeof(mem)));
    REQUIRE(w.isContentUnchanged(4));

    REQUIRE(cmp.size() == w.initLastSize());
    REQUIRE(memcmp(&cmp[1], mem, sizeof(mem)) == 0);
}

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    const AQItem *cmp = w.item.last();
    REQUIRE(w.item.write(&mem[0], 3));
    REQUIRE(w.item.write(&mem[3], 2));
    REQUIRE(w.item.write(&mem[5], 4));
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

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteBeforeEndExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    const AQItem *cmp = w.item.last();
    REQUIRE(w.item.write(w.initSize() - 1, &mem[0], 3));
    REQUIRE(w.item.write(&mem[3], 2));
    REQUIRE(w.item.write(&mem[5], 4));
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

//------------------------------------------------------------------------------
AQTEST_FORMAT(given_WriterItemExtendable_when_WriteAfterEndExtendsSize_then_WritePositionIncreased, AQ::OPTION_EXTENDABLE)
{
    ExtendableWitem w(aq);
    unsigned char mem[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    const AQItem *cmp = w.item.last();
    REQUIRE(w.item.write(w.initSize() + 1, &mem[0], 3));
    REQUIRE(w.item.write(&mem[3], 2));
    REQUIRE(w.item.write(&mem[5], 4));
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

// Determine if we have enough space or if we need to extend.  If we
// need to extend then do it or fail in the attempt.
//
// Example:
//   Item          #1 #2 #3 #4 #5
//   Capacity:    | 4| 8| 4|16| 4|
//   Size:        | 4| 8| 4|16| 2
//   Accumulator: | 0| 4|12|16|32|
//   rtotal:      |32|24|20| 4| 0|
//                                          rtotal + I(C) - off = avail - memSize       L(C) lastSize
//   (a) Item(I) #5, offset 1, size  3 -->       0 +    4 -   1 =  3    -       3 = 0   < L(C) => L(S) = L(C) - 0 = 4
//   (c) Item(I) #4, offset 6, size 14 -->       4 +   16 -   6 = 14    -      14 = 0   < L(C) => L(S) = L(C) - 0 = 4
//   (c) Item(I) #4, offset 6, size  9 -->       4 +   16 -   6 = 14    -       9 = 5  >= L(C) => L(S) = 0
//   (c) Item(I) #4, offset 6, size 10 -->       4 +   16 -   6 = 14    -      10 = 4  >= L(C) => L(S) = 0
//   (c) Item(I) #4, offset 6, size 11 -->       4 +   16 -   6 = 14    -      11 = 3   < L(C) => L(S) = L(C) - 3 = 1
//   (e) Item(I) #1, offset 1, size 35 -->      32 +    4 -   1 = 35    -      35 = 0   < L(C) => L(S) = L(C) - 0 = 4
//   (b) Item(I) #5, offset 1, size  4 -->       0 +    4 -   1 =  3    4 >  3 ==> EXTEND
//   (d) Item(I) #4, offset 7, size 14 -->       4 +   16 -   7 = 13   14 > 13 ==> EXTEND
//   (f) Item(I) #1, offset 2, size 35 -->      32 +    4 -   2 = 34   35 > 34 ==> EXTEND



//=============================== End of File ==================================
