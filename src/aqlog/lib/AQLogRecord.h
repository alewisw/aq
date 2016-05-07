#ifndef AQLOGRECORD_H
#define AQLOGRECORD_H
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

#include <vector>

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
 * Encapsuates a single log record retrieved from the log queue and decoded into
 * usable fields.
 */
class AQLogRecord
{
public:
    
    // Creates a new record.
    AQLogRecord(AQLogLevel_t level, const std::string& componentId, 
        const std::string& tagId, const std::string& file, 
        const std::string& message);

    // Constructs a record such that it is the exact copy of another record.
    AQLogRecord(const AQLogRecord& other);

    // Assigns the value of this record to exactly match another record.
    AQLogRecord& operator=(const AQLogRecord& other);

    // Destroys this filter.
    ~AQLogRecord(void);

private:

    // The log level for this record.
    AQLogLevel_t m_level;

    // The tier look-up for the log record at each look-up level.
    std::string m_tierId[AQLOG_LOOKUP_TIER_COUNT];

    // The message.
    std::string m_message;

public:

    // Gets the logging level for this record.
    AQLogLevel_t level(void) const { return m_level; }

    // Gets the identifier used at a particular filter tier 'idx'.
    const std::string& tierId(size_t idx) const { return m_tierId[idx]; }

    // Gets the component identifier for this record.
    const std::string& componentId(void) const { return m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID]; }

    // Gets the tag identifier for this record.
    const std::string& tagId(void) const { return m_tierId[AQLOG_LOOKUP_TIER_TAGID]; }

    // Gets the file identifier for this record.
    const std::string& file(void) const { return m_tierId[AQLOG_LOOKUP_TIER_FILE]; }
    
    // Gets the message from this log record.
    const std::string& message(void) const { return m_message; }

};




#endif
//=============================== End of File ==================================
