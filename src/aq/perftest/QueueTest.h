#ifndef QUEUETEST_H
#define QUEUETEST_H
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

// Forward declarations.
class IQueueProvider;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines a performance test that uses a queue with both readers and writers.
class QueueTest : public PerfTest
{
public:

    // Constructs a new queue-based test with the name 'name'.
    QueueTest(const std::string& name, IQueueProvider& queueProvider);

private:
    // No copy or assignment permitted.
    QueueTest(const QueueTest& other);
    QueueTest& operator=(const QueueTest& other);
public:

    // Destroys this thread overhead test.
    virtual ~QueueTest(void);

protected:

    IQueueProvider& queueProvider(void) { return m_queueProvider; }

    // Called before the test is run to setup the test in the sub-class.
    virtual void before(void);

    // Called before each iteration of the test.
    virtual void beforeIteration(void);

    // Called after the test is run to clean-up the test in the sub-class.
    virtual void after(void);

public:

    // Gets a description of the configuration of this test.
    virtual std::string config(void) const;

    // Gets a description of the resultant state for this test.
    virtual std::string results(void) const;

private:

    // The queue provider for this test.
    IQueueProvider& m_queueProvider;

};



#endif
//=============================== End of File ==================================
