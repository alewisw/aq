//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TestPointNotifier.h"

#include <cstring>

using namespace std;
using namespace aq;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TestPointNotifier::TestPointNotifier(int testPointCount, AQ *defaultQueue)
    : m_testPointCount(testPointCount)
    , m_defaultQueue(defaultQueue)
{
    m_testPoints = new TestPointRegistration[m_testPointCount];
    memset(m_testPoints, 0, sizeof(TestPointRegistration) * m_testPointCount);
}

//-----------------------------------------------------------------------------
TestPointNotifier::~TestPointNotifier(void)
{
    delete[] m_testPoints;
}

//------------------------------------------------------------------------------
void TestPointNotifier::registerTestPoint(int tp, 
    TestPointNotification_fn notification, AQ *queue, void *context)
{
    if (tp >= 0 && tp < m_testPointCount)
    {
        TestPointRegistration &reg = m_testPoints[tp];

        reg.notification = notification;
        reg.queue = queue == NULL ? m_defaultQueue : queue;
        reg.context = context;
    }
}

//------------------------------------------------------------------------------
void TestPointNotifier::testPoint(int tp)
{
    if (tp >= 0 && tp < m_testPointCount)
    {
        TestPointRegistration &reg = m_testPoints[tp];

        if (reg.notification != NULL)
        {
            TestPointNotification_fn notification = reg.notification;
            reg.notification = NULL;
            notification(reg.queue, reg.context);
        }
    }
}



//=============================== End of File ==================================
