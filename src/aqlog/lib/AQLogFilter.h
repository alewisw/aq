#ifndef AQLOGFILTER_H
#define AQLOGFILTER_H
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

#include <string>

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogHandler;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Defines a log filter.  A log filter is used to identify which log records
 * are passed to a log handler.
 */
class AQLogFilter
{
public:

    // Creates a new filter.  The filter must match the key1, key2, and
    // key3 strings as specified.  If any of the arguments are empty then
    // the rule is applied fully at the specified level.
    AQLogFilter(AQLogHandler& handler, AQLogLevel_t level, 
        const std::string& componentId, const std::string& tagId, 
        const std::string& file);

    // Constructs a filter such that it is the exact copy of another filter.
    AQLogFilter(const AQLogFilter& other);

    // Assigns the value of this filter to exactly match another filter.
    AQLogFilter& operator=(const AQLogFilter& other);

    // Destroys this filter.
    ~AQLogFilter(void);

private:

    // The handler that owns this filter.
    AQLogHandler *m_handler;

    // The log level for this filter.
    AQLogLevel_t m_level;

    // The three strings used for this filter.
    std::string m_tierId[AQLOG_LOOKUP_TIER_COUNT];

    // The number of keys in this filter.
    size_t m_tierIdCount;

public:

    // The handler that owns this filter.
    AQLogHandler& handler(void) const { return *m_handler; }

    // Gets one of the tier identifiers for this filter.  There are 
    // AQLOG_LOOKUP_TIER_COUNT identifiers available.
    const std::string& tierId(size_t idx) const { return m_tierId[idx]; }

    // The number of keys in this filter.
    size_t tierIdCount(void) const { return m_tierIdCount; }

    // Gets the logging level for this filter.
    AQLogLevel_t level(void) const { return m_level; }

};




#endif
//=============================== End of File ==================================
