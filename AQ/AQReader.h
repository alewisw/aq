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

// Implements the reader side of a Multi-Producer Allocating Concurrent queue.
// Given a single memory region only a single reader may read from the queue at
// any one time.  Moreover an object of this class can only be accessed from a
// single thread at any one time - it is not reentrant.
class AQReader : public AQ
{
public:

    // Constructs a queue reader object that uses the passed shared memory 
    // region 'mem' of total size 'memSize' bytes.
    //
    // The 'trace' argument is used for tracing and logging queue access.
    AQReader(void *mem, size_t memSz, aq::TraceBuffer *trace = NULL);

    // No implementation is defined for this function - readers must
    // never be copied as only one may ever exist for a given shared
    // memory region.
    AQReader(const AQReader& other);

    // No implementation is defined for this function - readers must
    // never be copied as only one may ever exist for a given shared
    // memory region.
    AQReader& operator=(const AQReader& other);

    // Destroys this queue reader.
    virtual ~AQReader(void);

    // Formats this shared memory using the page size (1 << 'pageSizeShift')
    // and the specified commit timeout 'commitTimeoutMs' measured in 
    // milliseconds.
    //
    // The 'options' field is a bit-mask that enables configurable options
    // on the format of the queue.  The options available are:
    //  OPTION_FLAG_CRC32 = protect each item with a CRC-32.
    bool format(int pageSizeShift, unsigned int commitTimeoutMs, unsigned int options = 0);

    // Returns a reference to an integer that changes as values are committed
    // into the queue.
    const volatile uint32_t& commitCounter(void) const;

    // Obtains the next item from the queue.  If there are no items in the 
    // queue available for processing then 'false' is returned.  If an item
    // is available its details are placed in 'item' and 'true' is returned.
    //
    // Once an item is returned it remains valid until release() is called
    // for the item.  If retreive() is called again before release() then
    // the next available item is returned (given that queue contains any
    // items).
    //
    // If the queue is not formatted then a AQUnformattedException is 
    // thrown.
    bool retrieve(AQItem& item);

    // Releases the passed item so that it can be discarded from the queue.
    //
    // If the passed item is invalid (i.e., was not obtained via retrieve(),
    // or has already been released) then a std::invalid_argument exception is
    // thrown.
    //
    // If the queue is not formatted then a AQUnformattedException is 
    // thrown.
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
