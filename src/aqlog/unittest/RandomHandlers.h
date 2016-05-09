#ifndef RANDOMHANDLERS_H
#define RANDOMHANDLERS_H
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

#include "LogLevelHash.h"




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestHandler;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------


// Used to add random handlers to a test in order to perturb the results.
class RandomHandlers
{
public:

    // Constructs a new random handler colection - there are 'n' handlers
    // added to 'hash'.
    RandomHandlers(LogLevelHash& hash, AQLogLevel_t maxLevel = AQLOG_LEVEL_TRACE, 
        size_t n = 100);

    // Destroys the handlers.
    ~RandomHandlers(void);

private:

    // The handlers that have been added.
    std::vector<TestHandler *> m_handlers;

};




#endif
//=============================== End of File ==================================
