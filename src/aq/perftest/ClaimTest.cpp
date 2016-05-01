//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "ClaimTest.h"

#include "IQueueProvider.h"

#include "IAQWriter.h"




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
ClaimTest::ClaimTest(const std::string& name, IQueueProvider& queueProvider,
    int threadCount)
    : QueueTest(name, queueProvider)
    , m_claimPerThread(queueProvider.usablePageCount() / (size_t)threadCount)
{
    for (int i = 0; i < threadCount; ++i)
    {
        addThread<ClaimTest>(&ClaimTest::threadClaim);
    }
}

//------------------------------------------------------------------------------
ClaimTest::~ClaimTest(void)
{
}

//------------------------------------------------------------------------------
void ClaimTest::threadClaim(void)
{
    AQWriterItem item;
    IAQWriter& writer = queueProvider().writer();
    for (size_t i = 0; i < m_claimPerThread; ++i)
    {
        writer.claim(item, 1);
    }
}




//=============================== End of File ==================================
