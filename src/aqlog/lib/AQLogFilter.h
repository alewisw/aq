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

// Defines a log filter.  The filter specifies the log level as applied at each
// filter level.
class AQLogFilter
{
public:

    // Creates a new filter.  The filter must match the key1, key2, and
    // key3 strings as specified.  If any of the arguments are empty then
    // the rule is applied fully at the specified level.
    AQLogFilter(AQLogHandler& handler, AQLogLevel_t level, const std::string& key1, 
                const std::string& key2, const std::string& key3);

    // Constructs a filter such that it is the exact copy of another filter.
    AQLogFilter(const AQLogFilter& other);

    // Assigns the value of this filter to exactly match another filter.
    AQLogFilter& operator=(const AQLogFilter& other);

    // Destroys this filter.
    ~AQLogFilter(void);

private:

    // The number of filter keys.
    static const uint32_t KEY_COUNT = 3;

    // The handler that owns this filter.
    AQLogHandler *m_handler;

    // The log level for this filter.
    AQLogLevel_t m_level;

    // The three strings used for this filter.
    std::string m_keys[KEY_COUNT];

    // The number of keys in this filter.
    size_t m_keyCount;

public:

    // The handler that owns this filter.
    AQLogHandler& handler(void) const { return *m_handler; }

    // Gets one of the keys for this filter.  There are KEY_COUNT keys in the
    // array.
    const std::string& key(size_t idx) const { return m_keys[idx]; }

    // The number of keys in this filter.
    size_t keyCount(void) const { return m_keyCount; }

    // Gets the logging level for this filter.
    AQLogLevel_t level(void) const { return m_level; }

};




#endif
//=============================== End of File ==================================
