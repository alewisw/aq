#ifndef LOGREADER_H
#define LOGREADER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogRecord.h"

#include "AQReader.h"

#include "Timer.h"

#include <deque>
#include <list>
#include <set>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class IAQSharedMemory;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// A LogReader is used to read log record written by a LogWriter into a shared
// memory queue.
namespace aqlog { class LogReader
{
private:

    // The maximum number of items to hold in the pending list.
    static const uint32_t PENDING_WINDOW_SIZE = 100;

    // The minimum amount of time to delay an item in the pending list.
    static const uint32_t PENDING_MINIMUM_WINDOW_MS = 100;

    // The maximum amount of time to delay an item in the pending list.
    static const uint32_t PENDING_MAXIMUM_WINDOW_MS = 200;

public:

    // Constructs a new log reader that uses a shared memory region to
    // communicate with all of the log writers.  Only a single log reader can
    // use any given shared memory region.
    LogReader(IAQSharedMemory& sm);

    // Destroys this log reader.
    ~LogReader(void);

private:
    // No implementation provided - log readers cannot be copied or 
    // assigned.
    LogReader(const LogReader& other);
    LogReader& operator=(const LogReader& other);

public:

    // Retrieves the next log record from this reader.  This return NULL if there
    // are no available records.
    //
    // The returned record must be passed to release() which it has been processed
    // or it will leak.
    //
    // On return maxRecallMs is the maximum number of milliseconds that the caller
    // can wait before calling retrieve() again.
    AQLogRecord *retrieve(uint32_t& maxRecallMs);

    // Releases the passed log record from this reader.  Throws an invalid_argument
    // exception is this record is not an outstanding one from this reader.
    void release(AQLogRecord *rec);

private:

    // Allocates a new AQLogRecord object and returns that object.
    AQLogRecord *alloc(void);

    // Frees the passed AQLogRecord object.
    void free(AQLogRecord *rec);

    // The AQReader used by this log reader.
    AQReader m_aq;

    // Counts the number of different population outcome events.
    uint32_t m_outcomeCount[AQLogRecord::POPULATE_OUTCOME_COUNT];

    // Log records that have been received but not yet returned - they are pending
    // reordering due to timestamps.
    std::list<AQLogRecord *> m_pending;

    // The set of log records returned but not yet free'd by calling release().
    std::set<AQLogRecord *> m_outstanding;

    // Log records that are not currently used - but can be allocated to future
    // processing.
    std::deque<AQLogRecord *> m_free;

public:

    // Returns the number of times a particular population outcome has occurred.
    uint32_t populationOutcomeCount(AQLogRecord::PopulateOutcome outcome) const 
    { 
        return m_outcomeCount[outcome]; 
    }

};}




#endif
//=============================== End of File ==================================
