#ifndef RELEASETEST_H
#define RELEASETEST_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "QueueTest.h"

#include "AQItem.h"




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

// Tests the performance of the release() call.
class ReleaseTest : public QueueTest
{
public:

    // Constructs a new retreive test that will run release() operations in 
    // a single thread taken from 'queueProvider'.
    ReleaseTest(const std::string& name, IQueueProvider& queueProvider);

private:
    // No copy or assignment permitted.
    ReleaseTest(const ReleaseTest& other);
    ReleaseTest& operator=(const ReleaseTest& other);
public:

    // Destroys this claim test.
    virtual ~ReleaseTest(void);

    // The total number of operations that were performed.
    virtual unsigned long totalOperationCount(void) const
    {
        return iterationCount() * m_items.size();
    }

protected:

    // Called before each iteration of the test.
    virtual void beforeIteration(void);

private:

    // The items to release.
    std::vector<AQItem> m_items;

    // Runs the retrieve test.
    void threadRelease(void);

};



#endif
//=============================== End of File ==================================
