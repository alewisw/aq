//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "RetrieveReleaseTest.h"

#include "IQueueProvider.h"

#include "IAQReader.h"
#include "IAQWriter.h"
#include "AQItem.h"

using namespace std;




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
RetrieveReleaseTest::RetrieveReleaseTest(const std::string& name, IQueueProvider& queueProvider)
    : QueueTest(name, queueProvider)
    , m_retrieveCount(queueProvider.usablePageCount())
{
    addThread<RetrieveReleaseTest>(&RetrieveReleaseTest::threadRetrieveRelease);
}

//------------------------------------------------------------------------------
RetrieveReleaseTest::~RetrieveReleaseTest(void)
{
}

//------------------------------------------------------------------------------
void RetrieveReleaseTest::beforeIteration(void)
{
    QueueTest::beforeIteration();

    IAQWriter& writer = queueProvider().writer();
    for (size_t i = 0; i < m_retrieveCount; ++i)
    {
        AQWriterItem item;
        writer.claim(item, 1);
        writer.commit(item);
    }
}

//------------------------------------------------------------------------------
void RetrieveReleaseTest::threadRetrieveRelease(void)
{
    AQItem item;
    IAQReader& reader = queueProvider().reader();
    for (size_t i = 0; i < m_retrieveCount; ++i)
    {
        reader.retrieve(item);
        reader.release(item);
    }
}




//=============================== End of File ==================================
