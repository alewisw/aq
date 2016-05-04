#ifndef AQLOGHANDLER_H
#define AQLOGHANDLER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogFilter.h"

#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogRecord;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines a log filter.  The filter specifies the log level as applied at each
// filter level.
class AQLogHandler
{
public:

    // Constructs a new log handler.
    AQLogHandler(void);

    // Destroys this log handler.
    virtual ~AQLogHandler(void);

    // Adds a filter to this handler.
    void addFilter(AQLogLevel_t level, const std::string& key1 = "",
        const std::string& key2 = "", const std::string& key3 = "");

    // Handles the passed log record.
    virtual void handle(const AQLogRecord& rec) = 0;

private:

    // The filters for this handler.
    std::vector<AQLogFilter> m_filters;

public:

    // The array of the filters that cause records to be stored in this handler.
    const std::vector<AQLogFilter>& filters(void) const { return m_filters; }

};




#endif
//=============================== End of File ==================================
