#ifndef LOGLEVELHASH_H
#define LOGLEVELHASH_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLog.h"

#include "HashFunction.h"

#include <list>
#include <map>
#include <set>

#include <stdint.h>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogFilter;
class AQLogHandler;
class AQLogRecord;

class IAQSharedMemory;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines the log level hash table writer.
namespace aqlog { class LogLevelHash
{
public:

    // Constructs a new log level hash in the memory 'hashMem'.  This memory must
    // be at least AQLOG_HASH_TABLE_WORDS uint32_t words in length.
    LogLevelHash(IAQSharedMemory& hashMem, HashFunction_fn hashFn = HashFunction::standard);

    // Destroys this log level hash.  This does not change the content of the hash
    // memory.
    ~LogLevelHash(void);

private:
    // Copy and assignment are not supported for log level hash due to the
    // use of shared memory.
    LogLevelHash(const LogLevelHash& other);
    LogLevelHash& operator=(const LogLevelHash& other);

public:

    // Freezes hash table recalculation.  The hash table is not updated as handlers 
    // are added and removed.  When unfreezeHash() is called, if any changes were
    // made that needed the table to be rebuilt, the it is rebuilt.
    void freezeHash(void);

    // Unfreezes the hash and rebuilds the table if required.
    void unfreezeHash(void);

    // Adds a log handler to this log level hash.  After calling this function the
    // handler may not be further modified.
    void addHandler(AQLogHandler *handler);

    // Removes a log handler from this log level hash.
    void removeHandler(AQLogHandler *handler);

    // Handles the passed logging record, passing it to the appropriate log handlers.
    void handle(const AQLogRecord& rec);

private:

    // The possible states for the hash rebuild freezer.
    enum FreezeState
    {
        // Freezing is off - rebuild inline.
        FREEZE_OFF,

        // Freezing is on but no change has been made that needs a rebuild.
        FREEZE_ON,

        // Freezing is on and a change has been made that needs a rebuild.
        FREEZE_REBUILD,
    };

    // The state of the hash table rebuild freezer.
    FreezeState m_freezeState;

    // Define the map of filters at a particular log key.
    struct FilterMap
    {
        // The set of filters at this log key sorted by log level (lowest first).
        std::list<const AQLogFilter *> m_handlers;

        // The map of filters that require further progression down the tree.
        std::map<std::string, FilterMap> m_children;

    };

    // Adds the passed filter 'filter' to the filter map. 
    void addFilter(const AQLogFilter *filter);

    // Removes the passed filter from tee filter map.
    void removeFilter(const AQLogFilter *filter);

    // Populates the handler set with all the handlers that match the given record
    // with the filter map which is located at the specified tier.
    void populateHandlers(std::set<AQLogHandler *>& handlers, const AQLogRecord& rec,
        FilterMap& fm, size_t tier);

    // Populates the hash with the filter specified in 'filter'.
    void populateHash(uint32_t *hashMem, const AQLogFilter& filter, uint32_t index, uint32_t tier);

    // Rebuilds the entire hash.
    void rebuildHash(void);

    // Repopulates the hash memory in 'hashMem' starting at filter 'fm'.
    void repopulateHash(uint32_t *hashMem, FilterMap& fm);

    // The collection of top-level filters.
    FilterMap m_filters;

    // The hash memory.
    IAQSharedMemory& m_hashMem;

    // The hash function.
    HashFunction_fn m_hashFn;

};}




#endif
//=============================== End of File ==================================
