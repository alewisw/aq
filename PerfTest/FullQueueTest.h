#ifndef FULLQUEUETEST_H
#define FULLQUEUETEST_H
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

// Tests the performance with one thread performing retrieve()/release() and 
// some number of threads performing claim()/commit().
class FullQueueTest : public QueueTest
{
public:

    // Constructs a new full queue test with 'claimCommitThreadCount' threads 
    // performing the claim/commit cycle and one thread performing the 
    // retrieve()/release().
    FullQueueTest(const std::string& name, IQueueProvider& queueProvider, 
        int claimCommitThreadCount, bool performMemcpy = false);

    // Destroys this claim test.
    virtual ~FullQueueTest(void);

    // The total number of operations that were performed.
    virtual unsigned long totalOperationCount(void) const 
    { 
        return iterationCount() * m_claimPerThread * (threadCount() - 1);
    }

private:

    // Random data used for memory copies.
    unsigned char *m_data;

    // The number of claim operations to perform per thread.
    const size_t m_claimPerThread;

    // Set to true to perform a memcpy() for each buffer retrieved.
    const bool m_performMemcpy;

    // The maximum offset into the data buffer to use.
    const size_t m_dataMaxOffset;

    // Runs the claim/commit operation per thread.
    void threadClaimCommit(void);

    // Runs the retrieve/release operation per thread.
    void threadRetrieveRelease(void);

};



#endif
//=============================== End of File ==================================
