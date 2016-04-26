#ifndef AQREADER_H
#define AQREADER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQ.h"




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
namespace aq
{
    class LinkedItemProcessor;
};
class AQItem;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Implements the reader side of a Multi-Producer Concurrent Allocating Queue.
 * Given a single memory region only a single reader may read from the queue at
 * any one time.  Moreover an object of this class can only be accessed from a
 * single thread at any one time - it is not reentrant.
 */
class AQReader : public AQ
{
public:

    /**
     * Constructs a queue reader object that uses the passed shared memory region mem
     * of total size memSize bytes.  This does not read or write the memory -
     * it just sets up the internal pointers and references.
     *
     * Only a single AQReader object can be constructed for any one shared memory
     * region.  Having more than one AQReader object results in undefined behavior.
     *
     * Before the queue can be accesed it must be formatted with AQReader::format().
     *
     * @param mem The memory address where the queue is stored.
     * @param memSize The total size of the memory region where the queue is stored.
     */
    AQReader(void *mem, size_t memSize);

    // As above with the addition of a tracing buffer that holds all queue access logs.
    // This is only used in the unit and stress tests to track queue accesses and help
    // debug issues.
    AQReader(void *mem, size_t memSize, aq::TraceBuffer *trace);

private:

    // No implementation is defined for this function - readers must
    // never be copied as only one may ever exist for a given shared
    // memory region.
    AQReader(const AQReader& other);

    // No implementation is defined for this function - readers must
    // never be copied as only one may ever exist for a given shared
    // memory region.
    AQReader& operator=(const AQReader& other);

public:

    /**
     * Destroys this queue reader.  Any outstanding AQItem objects otained by
     * calling retrieve() but not yet released through release() remain
     * as unreleased in the queue.  Accessing any of these objects result in
     * undefined behavior.
     *
     * The underlying memory of the queue is not impacted by this operation.
     */
    virtual ~AQReader(void);

    /**
     * Formats the shared memory for this queue so it can be used by the reader
     * and writers.
     *
     * @param pageSizeShift Configures the page size for the queue.  The page
     * size is set to 2^pageSizeShift; for example if pageSizeShift is 7 then 
     * the page size is 128 bytes.
     * @param commitTimeoutMs The maximum amount of time that an AQWriterItem
     * obtained from AQWriter::claim() can be held before calling 
     * AQWriter::commit().  This time window is how the queue handles writers
     * that terminate (i.e., crash or otherwise) while olding uncommitted
     * AQWriterItem objects.  Holding an item for long than this time can result
     * in it being returned as uncommitted (AQItem::isCommitted() returns false).
     * An application that writes to the memory of an AQWriterItem that was 
     * returned as uncommitted causes undefined behavior.  As such, for reliable
     * operation, writers must commit their items within the commitTimeoutMs 
     * time window after AQWriter::claim() returns the item.
     * @param options The set of options for this queue.  This is a bit-mask 
     * where the options are joined together by a logical OR operation.
     * Refer to the descriptions of AQ::OPTION_CRC3, AQ::OPTION_LINK_IDENTIFIER, 
     * and AQ::OPTION_EXTENDABLE for more information.
     * @returns True if the queue was formatted or false if it could not be formatted.
     * The queue formatting operation fails when there is not enough space in the queue
     * to setup for the specified configuration.
     */
    bool format(uint32_t pageSizeShift, uint32_t commitTimeoutMs, uint32_t options = 0);

    /**
     * Obtains a reference to a memory address that changes whenever an item is 
     * committed to the queue.  This can be used as a cheap method of polling for 
     * 'item available'.
     *
     * The main purpose of this is to implement efficient polling:
     * ~~~
     * void pollRetrieve(AQReader& reader, AQItem& item)
     * {
     *     const volatile uint32_t& commitCounter = reader.commitCounter();
     *     uint32_t count = commitCounter;
     *     while (!reader.retrieve(item))
     *     {
     *         while (count == commitCounter)
     *         {
     *             ... perform some other processing, sleep, etc ...
     *         }
     *         count = commitCounter;
     *     }
     * }
     * ~~~
     *
     * @returns A reference to an integer whose value changes whenever an item
     * is committed to the queue.
     */
    const volatile uint32_t& commitCounter(void) const;

    /**
     * Obtains the next item from the queue.  The item argument is populated
     * with the details of the returned item.  Once an item is returned it
     * remains valid until release() is called for the item.  If retreive() 
     * is called again before release() then/ the next available item is 
     * returned (given that queue contains any items).
     *
     * @param item The item object to fill with the detail of the retrieved
     * item.
     * @returns True if an item was obtained or false if no item was available
     * in the queue.If there are no items in the 
     * @throws AQUnformattedException When the queue is not formatted.
     */
    bool retrieve(AQItem& item);

    /**
     * Releases the passed item so that it can be discarded from the queue.
     * The item must have been previously obtained via a call to retrieve() and
     * not yet been released.
     *
     * Accessing any of the item memory once release() is called results in 
     * undefined behavior.
     * 
     * @param item The item to release.  When this function returns this item
     * is marked as not allocated (AQItem::isAllocated() returns false).
     * @throws std::invalid_argument The passed item was not obtained by
     * retrieve() or has already been released.
     */
    void release(AQItem& item);

private:

    // Performs a release on a single item given by 'item'.
    void releaseSingle(AQItem& item);

public:

    // Performs a partial release starting with the item at 'start' and ending 
    // at 'end'.  If 'start' is 0 then the commit initialisation
    // is performed if needed.
    //
    // This function exists to aid in unit testing.  Care must be taken that
    // all buffers are committed in order.
    void releaseExtendable(AQItem& item, size_t start, size_t end);

private:

    // Performs the internal item queue walk.  If 'item' is NULL then the 
    // queue is walked only so long as entries are being free'd from the queue;
    // the first time an entry is encountered that cannot be free'd (or the 
    // queue becomes empty) false is returned with no further action.
    //
    // If 'item' is non-NULL then the walk continues until an item is 
    // encountered that can be returned to the caller.  If an item is found
    // then 'true' is returned, otherwise 'false' is returned.
    bool walk(AQItem *item = NULL);

    // Called when walk() finds an item to return.  If 'item' is NULL then
    // no action is taken and false is returned.
    //
    // If 'item' is non-NULL it is filled out with the passed fields and
    // true is returned.
    bool walkEnd(AQItem *item, uint32_t ref, 
                 unsigned int advance, size_t memSize);

    // The linked item processor used by this reader.
    aq::LinkedItemProcessor *m_linkProcessor;

    // Defines the state of a page in the queue.
    struct PageState
    {
        // The timer start time in milliseconds.
        uint32_t timerStartMs;

        // Set to non-zero once the timer has started.
        uint32_t timerStarted : 1;

        // Set to non-zero once the timer has expired.
        uint32_t timerExpired : 1;

        // Set to non-zero once the item for this page has been retreived.
        uint32_t retrieved : 1;

        // The number of pages to skip to reach the next item.
        uint32_t skipCount : 29;

    };

    // Reflects the state of each page in the queue.  There is one entry in this
    // array for each page in the queue.
    //
    // The page state provides the mechanism for tracking orphan items and
    // discarding them or returning them as incomplete as well as allowing
    // retreive() to read out multiple items without any intervening
    // release().
    //
    // It is dynamically allocated based on the queue format (that is, number of
    // pages in the queue).
    PageState *m_pstate;


    // Defines all the available test points where event injection can occur.
public:

    // In release() before the control queue entry is written for the second time.
    static const int ReleaseBeforeWriteSecondCtrl = 0;

    // In retrieve()/release() before the control field for the item is about
    // to be written.
    static const int WalkBeforeWriteCtrl = ReleaseBeforeWriteSecondCtrl + 1;

    // In retrieve()/release() after the control field, but before the tail
    // reference has been updated.
    static const int WalkBeforeWriteTailRef = WalkBeforeWriteCtrl + 1;

    // Inside walk() immediatly after the N'th control word is read.  Thus:
    //    (WalkAfterReadCtrlN + 0) ==> after first read
    //    (WalkAfterReadCtrlN + 1) ==> after second read
    //                                 ...etc...
    static const int WalkAfterReadCtrlN = WalkBeforeWriteTailRef + 1;

    // Must the the last entry - the number of test points.
    static const int TestPointCount = WalkAfterReadCtrlN + 11;

};




#endif
//=============================== End of File ==================================
