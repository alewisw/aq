#ifndef RETRIEVERELEASETEST_H
#define RETRIEVERELEASETEST_H
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

#include "AQWriterItem.h"




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

// Tests the performance of the claim() call.
class RetrieveReleaseTest : public QueueTest
{
public:

    // Constructs a new retreive test that will run retrieve() operations in 
    // a single thread taken from 'queueProvider'.
    RetrieveReleaseTest(const std::string& name, IQueueProvider& queueProvider);

private:
    // No copy or assignment permitted.
    RetrieveReleaseTest(const RetrieveReleaseTest& other);
    RetrieveReleaseTest& operator=(const RetrieveReleaseTest& other);
public:

    // Destroys this claim test.
    virtual ~RetrieveReleaseTest(void);

    // The total number of operations that were performed.
    virtual unsigned long totalOperationCount(void) const
    {
        return iterationCount() * m_retrieveCount;
    }

protected:

    // Called before each iteration of the test.
    virtual void beforeIteration(void);

private:

    // The number of retrieve operations to perform.
    const size_t m_retrieveCount;

    // Runs the retrieve test.
    void threadRetrieveRelease(void);

};



#endif
//=============================== End of File ==================================
