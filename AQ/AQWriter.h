#ifndef AQWRITER_H
#define AQWRITER_H
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
class AQWriterItem;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Implements the writer side of a Multi-Producer Allocating Concurrent queue.
// Given a single memory region any number of writers in any number of threads
// may write into the queue at the same time.  The claim() and commit() 
// functions are thread-safe with respect to both reading and writing the queue
// with one exception.  If the queue is concurrently formatted by the reader
// then the behavior is undefined.
class AQWriter : public AQ
{
public:

    // Constructs a queue reader object that uses the passed shared memory 
    // region 'mem' of total size 'memSize' bytes.
    //
    // The 'trace' argument is used for tracing and logging queue access.
    AQWriter(void *mem, size_t memSize, aq::TraceBuffer *trace = NULL);

    // Constructs this queue producer as an exact copy of another queue.
    AQWriter(const AQWriter& other);

    // Assigns the value of this queue producer to exactly match another.
    AQWriter& operator=(const AQWriter& other);

    // Destroys this queue.
    virtual ~AQWriter(void);

    // Returns a reference to an integer that changes as values are free'd
    // from the queue.
    const volatile uint32_t& freeCounter(void) const;

private:

    // Converts the passed memory size as provided by the user to a capacity
    // as used internally by the XAQ.
    size_t sizeToCapacity(size_t size) const;

public:

    // Claims an item that can be written into by the producer.  The item 
    // consists of a memory region of size 'memSize' bytes.  If the region 
    // could be claimed then true is returned and 'item' updated to contain
    // the valid memory.  If the item could not be claimed then 'false' is 
    // returned and 'item' is marked invalid (exists() returns false).
    //
    // The value stored in the returned memory becomes available to the 
    // reader once the returned item is passed to commit().  It must be 
    // committed within the commit timeout period of the queue or it
    // is automatically reclaimed.  Writes to the item after the commit
    // timeout has expired can result in data corruption for other items.
    //
    // If the passed memory size is less than 1 or greater than 1 MB then
    // allocation is impossible and an invalid_argument exception is thrown.
    //
    // If the queue is not formatted then a AQUnformattedException is 
    // thrown.
    bool claim(AQWriterItem& item, size_t memSize);

    // Commits an item previously obtained via a call to claim() to
    // the queue so that it is available for consumption.  The caller must no 
    // longer access the originally claimed memory.
    //
    // Returns true if the commit succeeded or false if it fails; the commit
    // fails if the claimed item was treated as lost by the reader due
    // to begin held for an excessive period.
    //
    // If the passed item is invalid (i.e., is not currently claimed, is
    // no in the queue, is already committed) then an invalid_argument 
    // exception is thrown.
    //
    // If the queue is not formatted then a AQUnformattedException is 
    // thrown.
    bool commit(AQWriterItem& item);

private:

    // Performs a commit on a single item given by 'item'.
    bool commitSingle(AQItem& item);

public:

    // Performs a partial commit starting with the item at 'start' and ending 
    // at 'end'.  If 'start' is 0 then the commit initialisation
    // is performed if needed.
    //
    // This function exists to aid in unit testing.  Care must be taken that
    // all buffers are committed in order.
    bool commitExtendable(AQWriterItem& item, size_t start, size_t end);

private:

    // Configures all the link identifiers in the passed item.
    static void setExtendableLinkIdentifiers(AQWriterItem& item);

    // Defines all the available test points where event injection can occur.
public:

    // In claim() before the headRef field is written but after the
    // calculation has been made.
    static const int ClaimBeforeWriteHeadRef = 0;
        
    // In claim() before the control field for the skipped pages is
    // about the written.
    static const int ClaimBeforeWriteCtrlSkipPages = ClaimBeforeWriteHeadRef + 1;

    // In claim() before the control field for item is about to be
    // written.
    static const int ClaimBeforeWriteCtrl = ClaimBeforeWriteCtrlSkipPages + 1;

    // In commit() just before the control field is about to be written.
    static const int CommitBeforeWriteCtrl = ClaimBeforeWriteCtrl + 1;

    // Must the the last entry - the number of test points.
    static const int TestPointCount = CommitBeforeWriteCtrl + 1;

};




#endif
//=============================== End of File ==================================
