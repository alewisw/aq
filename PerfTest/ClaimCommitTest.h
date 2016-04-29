#ifndef CLAIMCOMMITTEST_H
#define CLAIMCOMMITTEST_H
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

// Tests the performance of the claim() then commit() calls.
class ClaimCommitTest : public QueueTest
{
public:

    // Constructs a new claim test that will run claim() operations in 
    // 'threadCount' threads taken from 'queueProvider'.
    ClaimCommitTest(const std::string& name, IQueueProvider& queueProvider, 
        int threadCount);

private:
    // No copy or assignment permitted.
    ClaimCommitTest(const ClaimCommitTest& other);
    ClaimCommitTest& operator=(const ClaimCommitTest& other);
public:

    // Destroys this claim test.
    virtual ~ClaimCommitTest(void);

    // The total number of operations that were performed.
    virtual unsigned long totalOperationCount(void) const 
    { 
        return iterationCount() * m_claimPerThread * threadCount();
    }

private:

    // The number of claim operations to perform per thread.
    const size_t m_claimPerThread;

    // Takes no action.
    void threadClaimCommit(void);

};



#endif
//=============================== End of File ==================================
