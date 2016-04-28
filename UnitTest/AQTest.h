#ifndef AQTEST_H
#define AQTEST_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Defines a test that takes an AQTest& object as an argument named 'aq'.  The
// variable length arguments are used as arguments to construct the AQTest
// object.
#define AQTEST(name)                    AQTEST_FORMAT(name, 0)

// As per AQTEST but allows the formatting options for the queue to be 
// configured.
#define AQTEST_FORMAT(name, options)                                            \
static void AQ_##name(AQTest& aq);                                              \
TEST(name)                                                                      \
{                                                                               \
    AQTest aq(options);                                                         \
    AQ_##name(aq);                                                              \
}                                                                               \
static void AQ_##name(AQTest& aq)



//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Data required for an AQ unit test.
class AQTest
{
private:

    // The shared memory region guard size.
    static const size_t SHM_GUARD_SIZE = 8;

    // The guard byte used in the shared memory.
    static const unsigned char SHM_GUARD_BYTE = 0xCD;

    // The number of pages in the shared memory.
    static const size_t PAGE_COUNT = 11;

    // The page size shift used for the shared memory.
    static const size_t PAGE_SIZE_SHIFT = 2;

    // The page size for the shared memory.
    static const size_t PAGE_SIZE = 1 << PAGE_SIZE_SHIFT;

    // The shared memory region guard size.
    static const size_t SHM_SIZE = sizeof(CtrlOverlay) + ((PAGE_COUNT - 1) * sizeof(uint32_t)) + PAGE_SIZE * PAGE_COUNT;

    // Additional shared memory size overhead introduced by needing the CRC32.
    static const size_t SHM_CRC32_OVERHEAD_SIZE = PAGE_COUNT * sizeof(uint32_t);

    // Additional shared memory size overhead introduced by needing link identifiers.
    static const size_t SHM_LINK_IDENTIFIER_OVERHEAD_SIZE = PAGE_COUNT * sizeof(uint32_t);

public:

    // The commit timeout value used for these tests.
    static const unsigned int COMMIT_TIMEOUT_MS = 1000;

    AQTest(unsigned int formatOptions = 0);
    AQTest(const AQTest& other);
    AQTest& operator=(const AQTest& other);
    ~AQTest(void);

private:

    // The formatting options.
    unsigned int m_formatOptions;

    // The shared memory region.
    unsigned char m_shm[SHM_GUARD_SIZE + SHM_SIZE + SHM_CRC32_OVERHEAD_SIZE + SHM_LINK_IDENTIFIER_OVERHEAD_SIZE + SHM_GUARD_SIZE];

    // The size of the start guard region.
    size_t m_shmStartGuardSize;

    // The size of the shared memory region.
    size_t m_shmSize;

    // The size of the end guard region.
    size_t m_shmEndGuardSize;

    // The random data available for writing.
    char m_data[SHM_SIZE * 2];

    // The dummy trace manager.
    TraceManager m_tm;

public:

    // The control overlay.
    aq::CtrlOverlay *ctrl;

    // The queue reader used during the tests.
    AQReader reader;

    // The queue writer used during the tests.
    AQWriter writer;

    // The trace buffer for use by the test cases.
    TraceBuffer *trace;

public:

    // Reformat the queue.
    void reformat(void);

    // Gets the page count and size for this test configuration.
    size_t pageCount(void) const { return PAGE_COUNT; }
    size_t pageSize(void) const { return PAGE_SIZE; }

    // Mutates the memory pointer of 'item'.
    void mutateItemMem(AQItem& item, const unsigned char *mem) const
    {
        item.m_mem = (unsigned char *)mem;
    }

    // Returns true if the shared memory region is valid with no memory overruns.
    bool isShmValid(void);

    // Returns true if the 'item' has been allocated to page number 'pageNum' and
    // is of size 'size'.
    bool isItemPage(const AQItem& item, uint32_t pageNum, size_t size);

    // Identical to 'item' above howere also verifies the released and/or committed state
    // of the page.
    bool isComittedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isUncomittedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isReleasedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isReleasedCommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isReleasedUncommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isUnreleasedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isUnreleasedCommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size);
    bool isUnreleasedUncommittedItemPage(const AQItem& item, uint32_t pageNum, size_t size);

    // Returns true if both a and b are allocated an they refer to different 
    // memory regions.
    bool areDifferentAllocatedItems(const AQItem& a, const AQItem& b);

    // Returns true if both a and b are allocated and are identical items.
    bool areIdenticalAllocatedItems(const AQItem& a, const AQItem& b, bool full = false);

    // Returns true if both a and b are allocated and are identical items.
    bool areFullyIdenticalAllocatedItems(const AQItem& a, const AQItem& b)
    {
        return areIdenticalAllocatedItems(a, b, true);
    }

    // Writes the content of 'item' with the random data starting at 'off' of
    // size 'size' bytes.
    bool appendData(AQWriterItem& item, size_t off, size_t size);

    // Asserts that the data in 'item' matches the random data starting at 'off;
    // of size 'size'.
    bool isItemData(const AQItem& item, size_t off, size_t size, bool released = false)
    {
        return isPartialItemData(item, off, size, 0xFFFFFFFE, released);
    }

    // Asserts that the data in 'item' matches the random data starting at 'off;
    // of size 'size'.
    bool isPartialItemData(const AQItem& item, size_t off, size_t size, size_t itemCount, bool released = false);

    // Asserts that the data in 'item' matches the data written with enqueue() for item
    // number 'n' with size 'size'.
    bool isEnqueueItemData(const AQItem& item, size_t n, size_t size = 0, bool released = false);
    
    // Asserts that the data in 'item' matches the random data starting at 'off;
    // of size 'size'.
    bool isReleasedItemData(const AQItem& item, size_t off, size_t size)
    {
        return isItemData(item, off, size, true);
    }

    // Asserts that the data in 'item' matches the random data starting at 'off;
    // of size 'size'.
    bool isReleasedPartialItemData(const AQItem& item, size_t off, size_t size, size_t itemCount)
    {
        return isPartialItemData(item, off, size, itemCount, true);
    }

    // Asserts that the data in 'item' matches the data written with enqueue() for item
    // number 'n' with size 'size'.
    bool isReleasedEnqueueItemData(const AQItem& item, size_t n, size_t size = 0)
    {
        return isEnqueueItemData(item, n, size, true);
    }


    // Advances the queue position by adding and removing 'count' items of 'size'
    // bytes, defaulting to a single page size.
    void advance(uint32_t count, size_t size = 0) { advanceOrEnqueue(count, size, true); }

    // Enqueues 'count' items of 'size' bytes, defaulting to a single page size.
    void enqueue(uint32_t count, size_t size = 0) { advanceOrEnqueue(count, size, false); }

private:

    // Enqueues 'count' items of 'size' bytes, defaulting to a single page size
    // when 'size' is 0.  If 'removeItems' is true each item is removed after
    // it is aq.enqueued.
    void advanceOrEnqueue(uint32_t count, size_t size, bool removeItems);


};




#endif
//=============================== End of File ==================================
