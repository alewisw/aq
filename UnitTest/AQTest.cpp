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

// Uses the CHECK() on __x, and sets __res to false if the check failed.
#define CHECK_AND_UPDATE(__res, __x)                                            \
do                                                                              \
{                                                                               \
    if (!(__x))                                                                 \
    {                                                                           \
        __res = false;                                                          \
    }                                                                           \
    CHECK(__x);                                                                 \
} while (0)




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
AQTest::AQTest(unsigned int formatOptions)
    : m_formatOptions(formatOptions)
    , m_shmStartGuardSize(SHM_GUARD_SIZE)
    , m_shmSize(SHM_SIZE + ((formatOptions & AQ::OPTION_CRC32) ? SHM_CRC32_OVERHEAD_SIZE : 0)
                         + ((formatOptions & (AQ::OPTION_LINK_IDENTIFIER | AQ::OPTION_EXTENDABLE)) ? SHM_LINK_IDENTIFIER_OVERHEAD_SIZE : 0))
    , m_shmEndGuardSize(SHM_GUARD_SIZE + SHM_SIZE + SHM_CRC32_OVERHEAD_SIZE + SHM_LINK_IDENTIFIER_OVERHEAD_SIZE - m_shmSize)
    , ctrl((aq::CtrlOverlay *)&m_shm[SHM_GUARD_SIZE])
    , reader(&m_shm[SHM_GUARD_SIZE], m_shmSize, m_tm.createBuffer("rdr"))
    , writer(&m_shm[SHM_GUARD_SIZE], m_shmSize, m_tm.createBuffer("wrt"))
    , trace(m_tm.createBuffer("tst"))

{
    // Install the guards.
    memset(m_shm, SHM_GUARD_BYTE, m_shmStartGuardSize);
    memset(&m_shm[m_shmStartGuardSize + m_shmSize], SHM_GUARD_BYTE, m_shmEndGuardSize);

    // Fill the rest with random data.
    srand(7);
    for (size_t i = m_shmStartGuardSize; i < m_shmStartGuardSize + m_shmSize; ++i)
    {
        m_shm[i] = 'A' + rand() % 26;
    }

    // Create the random data array.
    for (size_t i = 0; i < sizeof(m_data); ++i)
    {
        m_data[i] = 'A' + rand() % 26;
    }

    reformat();
}

//------------------------------------------------------------------------------
AQTest::~AQTest(void)
{
    CHECK(isShmValid());

    if (TEST_IS_FAILING())
    {
        m_tm.write(cout);
    }
}

//------------------------------------------------------------------------------
void AQTest::reformat(void)
{
    CHECK(reader.format(PAGE_SIZE_SHIFT, AQTest::COMMIT_TIMEOUT_MS - 25, m_formatOptions));

    CHECK(reader.isFormatted());
    CHECK(writer.isFormatted());

    CHECK(reader.pageCount() == PAGE_COUNT);
    CHECK(writer.pageCount() == PAGE_COUNT);

    CHECK(reader.pageSize() == PAGE_SIZE);
    CHECK(writer.pageSize() == PAGE_SIZE);

    CHECK(isShmValid());
}

//------------------------------------------------------------------------------
bool AQTest::isShmValid(void)
{
    for (size_t i = 0; i < m_shmStartGuardSize; ++i)
    {
        const unsigned char *headGuard = &m_shm[i];
        if (*headGuard != SHM_GUARD_BYTE)
        {
            return false;
        }
    }

    for (size_t i = m_shmStartGuardSize + m_shmSize; i < m_shmStartGuardSize + m_shmSize + m_shmEndGuardSize; ++i)
    {
        const unsigned char *tailGuard = &m_shm[i];
        if (*tailGuard != SHM_GUARD_BYTE)
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
bool AQTest::isItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = true;

    CHECK_AND_UPDATE(res, item.size() == size);

    if ((size_t)(&item[0]) >= (size_t)&m_shm[m_shmStartGuardSize]
        && (size_t)(&item[0]) < (size_t)&m_shm[m_shmStartGuardSize + m_shmSize])
    {
        // Directly calculate and compare page numbers.
        size_t itemOffset = (size_t)(&item[0]) - (size_t)&m_shm[m_shmStartGuardSize];
        size_t overlayOffset = itemOffset - sizeof(CtrlOverlay) - (pageCount() - 1) * sizeof(uint32_t);
        size_t alignMask = (1 << PAGE_SIZE_SHIFT) - 1;
        uint32_t itemPageNum = (uint32_t)(overlayOffset >> PAGE_SIZE_SHIFT);

        CHECK_AND_UPDATE(res, (overlayOffset & alignMask) == 0);
        CHECK_AND_UPDATE(res, pageNum == itemPageNum);
    }
    else
    {
        // Compare memory content.
        const void *cmpMem = ctrl->pageToMem(pageNum);
        CHECK_AND_UPDATE(res, memcmp(cmpMem, &item[0], item.size()) == 0);
    }


    return res;
}

//------------------------------------------------------------------------------
bool AQTest::isComittedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, item.isCommitted());
    
    return res;
}

//------------------------------------------------------------------------------
bool AQTest::isUncomittedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, !item.isCommitted());

    return res;
}

//------------------------------------------------------------------------------
bool AQTest::isReleasedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, item.isReleased());

    return res;
}


//------------------------------------------------------------------------------
bool AQTest::isReleasedCommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, item.isReleased());
    CHECK_AND_UPDATE(res, item.isCommitted());

    return res;
}


//------------------------------------------------------------------------------
bool AQTest::isReleasedUncommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, item.isReleased());
    CHECK_AND_UPDATE(res, !item.isCommitted());

    return res;
}


//------------------------------------------------------------------------------
bool AQTest::isUnreleasedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, !item.isReleased());

    return res;
}


//------------------------------------------------------------------------------
bool AQTest::isUnreleasedCommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, !item.isReleased());
    CHECK_AND_UPDATE(res, item.isCommitted());

    return res;
}


//------------------------------------------------------------------------------
bool AQTest::isUnreleasedUncommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size)
{
    bool res = isItemPage(item, pageNum, size);

    CHECK_AND_UPDATE(res, !item.isReleased());
    CHECK_AND_UPDATE(res, !item.isCommitted());

    return res;
}

//------------------------------------------------------------------------------
bool AQTest::areDifferentAllocatedItems(const AQItem& a, const AQItem& b)
{
    bool res = true;

    const AQItem *ai = &a;
    const AQItem *bi = &b;
    for (; ai != NULL && bi != NULL; ai = ai->next(), bi = bi->next())
    {
        CHECK_AND_UPDATE(res, ai->isAllocated());
        CHECK_AND_UPDATE(res, bi->isAllocated());
        CHECK_AND_UPDATE(res, &(*ai)[0] != &(*bi)[0]);
    }

    return res;
}

//------------------------------------------------------------------------------
bool AQTest::areIdenticalAllocatedItems(const AQItem& a, const AQItem& b, bool full)
{
    bool res = true;

    const AQItem *ai = &a;
    const AQItem *bi = &b;
    const AQItem *aiPrev = NULL;
    const AQItem *biPrev = NULL;
    for (; ai != NULL && bi != NULL; ai = ai->next(), bi = bi->next())
    {
        CHECK_AND_UPDATE(res, ai->isAllocated());
        CHECK_AND_UPDATE(res, bi->isAllocated());

        CHECK_AND_UPDATE(res, ai->first() == &a);
        CHECK_AND_UPDATE(res, bi->first() == &b);

        if (aiPrev != NULL)
        {
            CHECK_AND_UPDATE(res, ai->prev() == aiPrev);
            CHECK_AND_UPDATE(res, bi->prev() == biPrev);
        }

        CHECK_AND_UPDATE(res, &(*ai)[0] == &(*bi)[0]);
        CHECK_AND_UPDATE(res, ai->size() == bi->size());
        CHECK_AND_UPDATE(res, ai->capacity() == bi->capacity());
        CHECK_AND_UPDATE(res, ai->queueIdentifier() == bi->queueIdentifier());

        if (full)
        {
            CHECK_AND_UPDATE(res, ai->linkIdentifier() == bi->linkIdentifier());
            CHECK_AND_UPDATE(res, ai->isChecksumValid() == bi->isChecksumValid());
            CHECK_AND_UPDATE(res, ai->isCommitted() == bi->isCommitted());
            CHECK_AND_UPDATE(res, ai->isReleased() == bi->isReleased());
            CHECK_AND_UPDATE(res, ai->ctrl() == bi->ctrl());
        }

        aiPrev = ai;
        biPrev = bi;
    }
    CHECK_AND_UPDATE(res, ai == NULL);
    CHECK_AND_UPDATE(res, bi == NULL);

    // Now check that 'last' is correct.
    CHECK_AND_UPDATE(res, a.last() == aiPrev);
    CHECK_AND_UPDATE(res, b.last() == biPrev);
    
    return res;
}

//------------------------------------------------------------------------------
bool AQTest::appendData(AQWriterItem& item, size_t off, size_t size)
{
    if (writer.isExtendable())
    {
        return writer.append(item, &m_data[off], size);
    }
    else
    {
        if (size > item.size())
        {
            return false;
        }
        memcpy(&item[0], &m_data[off], size);
        return true;
    }
}

//------------------------------------------------------------------------------
bool AQTest::isPartialItemData(const AQItem& item, size_t off, size_t size, 
    size_t itemCount, bool released)
{
    bool res = true;

    CHECK_AND_UPDATE(res, item.isReleased() == released);
    size_t p = 0;
    size_t count = 0;
    for (const AQItem *curr = &item; curr != NULL; curr = curr->next())
    {
        CHECK_AND_UPDATE(res, p + curr->size() <= size);
        if (count < itemCount)
        {
            CHECK_AND_UPDATE(res, memcmp(&(*curr)[0], &m_data[off + p], curr->size()) == 0);
        }
        p += curr->size();

        count++;
    }
    CHECK_AND_UPDATE(res, p == size);

    return res;
}

//------------------------------------------------------------------------------
bool AQTest::isEnqueueItemData(const AQItem& item, size_t n, size_t size, bool released)
{
    if (size == 0)
    {
        size = pageSize();
    }

    size_t pos = 0;
    for (size_t i = 0; i < n; ++i)
    {
        pos += size;
        if (pos + size > sizeof(m_data))
        {
            pos = pos % (sizeof(m_data) - size);
        }
    }
    return isItemData(item, pos, size, released);
}

//------------------------------------------------------------------------------
void AQTest::advanceOrEnqueue(uint32_t count, size_t size, bool removeItems)
{
    if (size == 0)
    {
        size = pageSize();
    }

    size_t pos = 0;
    for (uint32_t i = 0; i < count; ++i)
    {
        AQWriterItem witem;
        CHECK(writer.claim(witem, size));
        CHECK(appendData(witem, pos, size));
        CHECK(writer.commit(witem));

        if (removeItems)
        {
            AQItem ritem;
            CHECK(reader.retrieve(ritem));
            CHECK(isItemData(ritem, pos, size));
            reader.release(ritem);
        }

        pos += size;
        if (pos + size > sizeof(m_data))
        {
            pos = pos % (sizeof(m_data) - size);
        }
    }
}




//=============================== End of File ==================================
