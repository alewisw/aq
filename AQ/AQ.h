#ifndef AQ_H
#define AQ_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <cstdint>
#include <string>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------

// Forwrd declarations.
namespace aq 
{
    class TestPointNotifier;
    class TraceBuffer;
    struct CtrlOverlay;
}
class AQItem;




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Base class for a Multi-Producer Allocating Concurrent Queue.  This gives 
 * basic information about the queue and encapsulates its memory, however it 
 * does not provide any mechanism to read or write the queue.  This is done by
 * constructing any number of AQWriter objects for writing, and one
 * AQReader object for reading the queue.
 */
class AQ
{
    // The snapshot class may access the m_ctrl and m_memSize fields.
    friend class AQSnapshot;

    // The trace buffer captures data directly from within the MPAC queue for
    // logging purposes.
    friend class aq::TraceBuffer;

protected:

    /**
     * Constructs a queue object that uses the passed shared memory region mem
     * of total size memSize bytes.  This does not read or write the memory -
     * it just sets up the internal pointers and references.
     *
     * @param testPointCount This argument specifies the number of test points
     * used by the automated unit tests availableSize for this particular interface
     * to the queue.
     * @param mem The memory address where the queue is stored.
     * @param memSize The total size of the memory region where the queue is stored.
     * @param trace An optional tracing buffer that holds all queue access logs.  This
     * is only used in the unit and stress tests to track queue accesses and help
     * debug issues.
     */
    AQ(int testPointCount, void *mem, size_t memSize, 
       aq::TraceBuffer *trace = NULL);

    /**
     * Constructs this queue such that it references exactly the same underlying
     * memory range as another queue.
     * @param other The other queue whose memory range is to be used by this queue.
     */
    AQ(const AQ& other);

    /**
     * Assigns the underlying memory range of this queue to exactly match the shared
     * memory range of another queue.
     * @param other The other queue whose memory range is to be used by this queue.
     */
    AQ& operator=(const AQ& other);

    /**
     * Destroys this queue.  The underlying memory is not impacted by this operation.
     */
    virtual ~AQ(void);

protected:

    // The trace buffer for this queue.
    aq::TraceBuffer *m_trace;

public:

    /**
     * When formatting a queue set this flag to enable CRC-32 checksum calculation
     * and validation over each item in the queue.
     */
    static const unsigned int OPTION_CRC32 = 1 << 0;

    /**
     * When formatting a queue set this flag to attach a link identifier to each item.
     * This allows the AQWriterItem::setLinkIdentifier() function to be called, and it
     * value to be available on the retreived item via AQItem::linkIdentifier().  This
     * functionality is only available if OPTION_EXPANDABLE is not also configured as
     * OPTION_EXPANDABLE uses the link identifier to manage its linked-list of items.
     */
    static const unsigned int OPTION_LINK_IDENTIFIER = 1 << 1;

    /**
     * When formatting a queue set this flag to allow items to be extended in a linked
     * list.  This is generally only suitable when the full length of the item cannot
     * be known when AQWriter::claim() is called.  This function uses the link identifier
     * and will override any value set with AQWriterItem::setLinkIdentifier().
     */
    static const unsigned int OPTION_EXTENDABLE = 1 << 2;

public:

    /**
     * Determines if the memory for this queue has been correctly formatted.  It is not
     * possible to access a queue unless the memory has been formatted.
     *
     * @returns true if the queue memory is correctly formatted or false if the
     * queue memory is not formatted.
     */
    bool isFormatted(void) const;

    /**
     * Determines if this queue has been formatted in extendable mode.  In extendable 
     * mode (AQ::OPTION_EXTENDABLE set when AQReader::format() was called) items can be 
     * extended in a linked-list arrangement.
     *
     * @returns True if the queue is formatted in extendable mode, false if it is not
     * formatted or was not formatted in extendable mode.
     */
    bool isExtendable(void) const;

    /**
     * Obtains the size of the memory region containing the queue.
     *
     * @returns The size, in bytes, of the memory region that conatins the queue.
     */
    size_t memorySize(void) const { return m_memSize;  }

    /**
     * Obtains the size of each page in the queue.  When allocating memory 
     * from the queue using an integer multiple of pageSize() provides the most efficient 
     * allocation policy.
     *
     * @returns The size, in bytes, of each page in the queue.  If the queue
     * is not formatted then 0 is returned.
     */
    size_t pageSize(void) const;

    /**
     * Obtains the number of pages in the queue.
     *
     * @returns The number of pages in the queue.  If the queue is not formatted
     * then 0 is returned.
     */
    size_t pageCount(void) const;

    /**
     * Obtains the current maximum size of a single AQWriter::claim() request.
     * This may be less than the actual amount of space avaiable in the queue
     * because this returns the maximum contiguous allocation.  If the queue
     * has been split (i.e, there are two unused sections) it will be the largest
     * of the two sections.
     *
     * @returns The maximum number of contiguous bytes that can be allocated from
     * the queue at the current time.
     */
    size_t availableSize(void) const;

    /**
     * Obtains the number of times contention has occurred during AQWriter::claim()
     * calls.  Contention occurs when two threads call AQWriter::claim() at the 
     * same time resulting in only one thread succeeding, and the second thread
     * needing to perform a retry.  Contention can be seen as wasted time - so the
     * higher the contention the more wasted CPU cycles are occuring when claiming
     * items from the queue.
     *
     * @returns The monotonically increasing contention counter.  When the counter
     * reaches its limit (4,294,967,295) it rotates back to 0.
     */
    uint32_t claimContentionCount(void) const;

protected:

    // Throws an AQUnformattedException if this queue is not headerXref; if
    // the queue is headerXref returns the m_ctrl object.
    aq::CtrlOverlay *ctrlThrowOnUnformatted(const char *func) const;

    // Defines the control region of the shared memory.
    aq::CtrlOverlay *m_ctrl;

    // The total size, as passed into this class, of the shared memory.
    size_t m_memSize;


    // Test point management - only used as part of the automated unit testing
    // framework.
#ifdef AQ_TEST_POINT
protected:
    aq::TestPointNotifier *m_tpn;
    void testPoint(int tp);
public:
    aq::TestPointNotifier *testPointNotifier(void) const { return m_tpn;  }
#else
protected:
    void testPoint(int tp) { }
#endif

};




#endif
//=============================== End of File ==================================
