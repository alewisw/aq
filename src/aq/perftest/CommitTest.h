#ifndef COMMITTEST_H
#define COMMITTEST_H
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
class CommitTest : public QueueTest
{
public:

    // Constructs a new claim test that will run claim() operations in 
    // 'threadCount' threads taken from 'queueProvider'.
    CommitTest(const std::string& name, IQueueProvider& queueProvider, 
        int threadCount);

private:
    // No copy or assignment permitted.
    CommitTest(const CommitTest& other);
    CommitTest& operator=(const CommitTest& other);
public:

    // Destroys this claim test.
    virtual ~CommitTest(void);

    // The total number of operations that were performed.
    virtual unsigned long totalOperationCount(void) const 
    { 
        return iterationCount() * m_commitPerThread * threadCount();
    }

protected:

    // Called before each iteration of the test.
    virtual void beforeIteration(void);

private:

    // The number of claim operations to perform per thread.
    const size_t m_commitPerThread;

    // The list of items.
    std::vector<AQWriterItem> *m_items;

    // Takes no action.
    void threadCommit(std::vector<AQWriterItem>& items);

};



#endif
//=============================== End of File ==================================
