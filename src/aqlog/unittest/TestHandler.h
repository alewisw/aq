#ifndef TESTHANDLER_H
#define TESTHANDLER_H
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
#include "AQLogHandler.h"

#include <string>



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

// Defines the handler we use to run these tests.
class TestHandler : public AQLogHandler
{
public:

    // Constructs a new test handler.
    TestHandler(void);

    // Constructs a test handler with a filter as configured.
    TestHandler(AQLogLevel_t level, std::string tier1 = "",
        std::string tier2 = "", std::string tier3 = "");

    // Destroys this test handler.
    virtual ~TestHandler(void);

    // Handles the passed log record.
    virtual void handle(const AQLogRecord& rec);

private:

    // The log records handled.
    std::vector<const AQLogRecord *> m_records;

public:

    // Gets one of the records from this log level hash.
    const AQLogRecord *record(size_t idx) const;

};




#endif
//=============================== End of File ==================================
