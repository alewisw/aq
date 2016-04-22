//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "CommitTest.h"

#include "IQueueProvider.h"

#include "IAQWriter.h"

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
CommitTest::CommitTest(const std::string& name, IQueueProvider& queueProvider,
    int threadCount)
    : QueueTest(name, queueProvider)
    , m_commitPerThread(queueProvider.usablePageCount() / (size_t)threadCount)
{
    m_items = new vector<AQWriterItem>[threadCount];
    for (int i = 0; i < threadCount; ++i)
    {
        m_items[i].resize(m_commitPerThread);
        addThread<CommitTest, vector<AQWriterItem> >(&CommitTest::threadCommit, m_items[i]);
    }
}

//------------------------------------------------------------------------------
CommitTest::~CommitTest(void)
{
    delete[] m_items;
}

//------------------------------------------------------------------------------
void CommitTest::beforeIteration(void)
{
    QueueTest::beforeIteration();

    IAQWriter& writer = queueProvider().writer();
    for (size_t i = 0; i < threadCount(); ++i)
    {
        for (size_t j = 0; j < m_items[i].size(); ++j)
        {
            writer.claim(m_items[i][j], 1);
        }
    }
}

//------------------------------------------------------------------------------
void CommitTest::threadCommit(vector<AQWriterItem>& items)
{
    AQWriterItem item;
    IAQWriter& writer = queueProvider().writer();
    for (size_t i = 0; i < items.size(); ++i)
    {
        writer.commit(items[i]); 
    }
}




//=============================== End of File ==================================
