//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "FullQueueTest.h"

#include "IQueueProvider.h"

#include "Prng.h"

#include "IAQReader.h"
#include "IAQWriter.h"
#include "AQItem.h"

#include <string.h>




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
FullQueueTest::FullQueueTest(const std::string& name, IQueueProvider& queueProvider,
    int claimCommitThreadCount, bool performMemcpy)
    : QueueTest(name, queueProvider)
    , m_claimPerThread(queueProvider.usablePageCount() / (size_t)claimCommitThreadCount)
    , m_performMemcpy(performMemcpy)
    , m_dataMaxOffset(queueProvider.pageSize())
{
    for (int i = 0; i < claimCommitThreadCount; ++i)
    {
        addThread<FullQueueTest>(&FullQueueTest::threadClaimCommit);
    }
    addThread<FullQueueTest>(&FullQueueTest::threadRetrieveRelease);

    size_t dataSize = queueProvider.pageSize() + m_dataMaxOffset;
    m_data = new unsigned char[dataSize];
    Prng prng(1);
    for (size_t i = 0; i < dataSize; ++i)
    {
        m_data[i] = prng.next() & 0xFF;
    }
}

//------------------------------------------------------------------------------
FullQueueTest::~FullQueueTest(void)
{
    delete[] m_data;
}

//------------------------------------------------------------------------------
void FullQueueTest::threadClaimCommit(void)
{
    AQWriterItem item;
    IAQWriter& writer = queueProvider().writer();
    for (size_t i = 0; i < m_claimPerThread; ++i)
    {
        if (writer.claim(item, 1))
        {
            if (m_performMemcpy)
            {
                memcpy(&item[0], &m_data[i % m_dataMaxOffset], item.size());
            }
            writer.commit(item);
        }
    }
}

//------------------------------------------------------------------------------
void FullQueueTest::threadRetrieveRelease(void)
{
    AQItem item;
    IAQReader& reader = queueProvider().reader();
    size_t count = m_claimPerThread * (threadCount() - 1);
    while (count > 0)
    {
        if (reader.retrieve(item))
        {
            if (m_performMemcpy)
            {
                memcpy(&m_data[count % m_dataMaxOffset], &item[0], item.size());
            }
            reader.release(item);
            count--;
        }
    }

}




//=============================== End of File ==================================
