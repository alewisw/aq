#ifndef TESTPOINTNOTIFIER_H
#define TESTPOINTNOTIFIER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdlib.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------

// Forward declarations.
class AQ;




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines a test point notifier class - this class is used by multi-producer, 
// single-consumer shared memory queues and related to components to generate
// notifications callbacks whenever particular test points are reached.
namespace aq { class TestPointNotifier
{
public:

    // Constructs a new notifier with no registered callbacks.  The argument
    // 'defaultQueue' is passed to callbacks that don't provide their own queue.
    TestPointNotifier(int testPointCount, AQ *defaultQueue = NULL);

    // Not implemented - copying and assignment not supported.
    TestPointNotifier(const TestPointNotifier& other);
    TestPointNotifier& operator=(const TestPointNotifier& other);

    // Destroys this notifier.
    virtual ~TestPointNotifier(void);

    // Defines the function pointer template - that is, the function called 
    // when a test point is encountered.
    typedef void(*TestPointNotification_fn)(AQ *queue, void *context);

    // Registers a test point notification callback function so that 
    // 'notification' is called when 'tp' is reached.  The queue argument
    // passed to notification is 'queue' or the default for this notifier if
    // 'queue' is NULL.  The context argument is 'context'.
    void registerTestPoint(int tp, TestPointNotification_fn notification, 
        AQ *queue = NULL, void *context = NULL);

private:

    // Defines a test point registration function.
    struct TestPointRegistration
    {
        // The function to call.
        TestPointNotification_fn notification;

        // The queue argument to pass to the function.
        AQ *queue;

        // Additional context to pass to the function.
        void *context;
    };

    // The number of test points.
    int m_testPointCount;

    // The set of test point registrations on this queue.
    TestPointRegistration *m_testPoints;

    // The default notification queue.
    AQ *m_defaultQueue;

public:

    int testPointCount(void) const { return m_testPointCount; }

    // Called when the queue algorithm reaches the test point specified by 'tp'.
    void testPoint(int tp);

};}




#endif
//=============================== End of File ==================================
