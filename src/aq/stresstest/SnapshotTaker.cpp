//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "SnapshotTaker.h"

#include "Timer.h"

#include <string.h>
#include <sstream>
#include <stdexcept>

using namespace aqosa;
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
SnapshotTaker::SnapshotTaker(AQ &queue, int threadNum, unsigned int maxSnapPeriodMs,
    TraceManager *trace)
    : m_queue(queue)
    , m_prng(threadNum)
    , m_maxSnapPeriodMs(maxSnapPeriodMs)
    , m_trace(createTrace(trace, threadNum, "s"))
{
    memset(m_stats, 0, sizeof(m_stats));
}

//------------------------------------------------------------------------------
TraceBuffer *SnapshotTaker::createTrace(TraceManager *trace, int threadNum, 
    const char *prefix)
{
    if (trace == NULL)
    {
        return NULL;
    }
    else
    {
        ostringstream ss;

        ss << prefix << threadNum;
        return trace->createBuffer(ss.str());
    }
}

//------------------------------------------------------------------------------
SnapshotTaker::~SnapshotTaker(void)
{
    clearSnapshots();
}

//------------------------------------------------------------------------------
void SnapshotTaker::run(void)
{
    for (;;)
    {
        abortIfStop();
        yieldMs(m_prng.next() % m_maxSnapPeriodMs);
        abortIfStop();
        takeSnapshot();
    }
}

//------------------------------------------------------------------------------
void SnapshotTaker::takeSnapshot(void)
{
    uint32_t startMs = Timer::start();
    AQSnapshot *snapshot = new AQSnapshot(m_queue, m_trace);
    uint32_t elapsedMs = Timer::elapsed(startMs);

    // Count the number of complete records in the snapshot.
    unsigned int completeCount = 0;
    for (size_t i = 0; i < snapshot->size(); ++i)
    {
        if ((*snapshot)[i].isCommitted())
        {
            completeCount++;
        }
    }
    
    m_snapshots.push_back(snapshot);

    incrementStat(StatisticSnapshotsTaken);
    incrementStat(StatisticTotalRecords, completeCount);
    incrementStat(StatisticTotalSnapshotTimeMs, elapsedMs);
    if (getStat(StatisticSnapshotsTaken) == 1)
    {
        setStat(StatisticMinimumRecords, completeCount);
        setStat(StatisticMaximumRecords, completeCount);
        setStat(StatisticMaxSnapshotTimeMs, elapsedMs);
    }
    else
    {
        minimizeStat(StatisticMinimumRecords, completeCount);
        maximizeStat(StatisticMaximumRecords, completeCount);
        maximizeStat(StatisticMaxSnapshotTimeMs, elapsedMs);
    }
}

//------------------------------------------------------------------------------
void SnapshotTaker::clearSnapshots(void)
{
    for (size_t i = 0; i < m_snapshots.size(); ++i)
    {
        delete m_snapshots[i];
    }
    m_snapshots.clear();
}




//=============================== End of File ==================================
