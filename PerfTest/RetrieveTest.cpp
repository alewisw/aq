//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "RetrieveTest.h"

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
RetrieveTest::RetrieveTest(const std::string& name, IQueueProvider& queueProvider)
    : QueueTest(name, queueProvider)
    , m_retrieveCount(queueProvider.usablePageCount())
{
    addThread<RetrieveTest>(&RetrieveTest::threadRetrieve);
}

//------------------------------------------------------------------------------
RetrieveTest::~RetrieveTest(void)
{
}

//------------------------------------------------------------------------------
void RetrieveTest::beforeIteration(void)
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
void RetrieveTest::threadRetrieve(void)
{
    AQItem item;
    IAQReader& reader = queueProvider().reader();
    for (size_t i = 0; i < m_retrieveCount; ++i)
    {
        reader.retrieve(item);
    }
}




//=============================== End of File ==================================
