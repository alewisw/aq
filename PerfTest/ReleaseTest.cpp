//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "ReleaseTest.h"

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
ReleaseTest::ReleaseTest(const std::string& name, IQueueProvider& queueProvider)
    : QueueTest(name, queueProvider)
{
    m_items.resize(queueProvider.usablePageCount());
    addThread<ReleaseTest>(&ReleaseTest::threadRelease);
}

//------------------------------------------------------------------------------
ReleaseTest::~ReleaseTest(void)
{
}

//------------------------------------------------------------------------------
void ReleaseTest::beforeIteration(void)
{
    QueueTest::beforeIteration();

    IAQWriter& writer = queueProvider().writer();
    IAQReader& reader = queueProvider().reader();
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        AQWriterItem item;
        writer.claim(item, 1);
        writer.commit(item);

        reader.retrieve(m_items[i]);
    }
}

//------------------------------------------------------------------------------
void ReleaseTest::threadRelease(void)
{
    IAQReader& reader = queueProvider().reader();
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        reader.release(m_items[i]);
    }
}




//=============================== End of File ==================================
