#ifndef THREADOVERHEADTEST_H
#define THREADOVERHEADTEST_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "PerfTest.h"




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines a performance test that just starts and terminates a number of 
// threads to verify the overhead in just launching and stopping threads.
class ThreadOverheadTest : public PerfTest
{
public:

    // Constructs a new thread overhead test with 'threadCount' threads.
    ThreadOverheadTest(int threadCount);

    // Destroys this thread overhead test.
    virtual ~ThreadOverheadTest(void);

private:

    // Takes no action.
    void threadDoNothing(void);

};



#endif
//=============================== End of File ==================================
