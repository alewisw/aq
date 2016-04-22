#ifndef SNAPSHOTTAKER_H
#define SNAPSHOTTAKER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "WorkerThread.h"
#include "Prng.h"

#include "AQSnapshot.h"
#include "TraceManager.h"

#include <vector>

using namespace aq;




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates the logic for a thread that takes periodic snapshots of an 
// AQ.
class SnapshotTaker : public WorkerThread
{
public:

    // Constructs a new snapshot taker using the passed queue.  The snapshot
    // taker is allocated thread number 'threadNum'.
    //
    // If <trace> is non-NULL a trace buffer is created for the queue.
    SnapshotTaker(AQ& queue, int threadNum, unsigned int maxSnapPeriodMs, 
        TraceManager *trace);

private:

    // Creates a trace buffer using the passed trace manager and thread
    // number.
    static TraceBuffer *createTrace(TraceManager *trace, int threadNum, 
        const char *prefix);

public:

    // Destroys this thread.
    virtual ~SnapshotTaker(void);

    // Not implmented - cannot be copied or assigned.
    SnapshotTaker(const SnapshotTaker& other);
    SnapshotTaker& operator=(const SnapshotTaker& other);

protected:
    
    // Runs the snapshot taking thread.
    virtual void run(void);

private:

    // Takes the actual snapshot.
    void takeSnapshot(void);

    // The queue which we snapshot.
    AQ& m_queue;

    // The PRNG used for commit ordering.
    Prng m_prng;

    // The maximum snapshot period in milliseconds.
    unsigned int m_maxSnapPeriodMs;

    // The tracing buffer used by this snapshot taker.
    TraceBuffer *m_trace;

    // The taken snapshots.
    std::vector<AQSnapshot *> m_snapshots;

public:

    // Clears all the taken snapshots.
    void clearSnapshots(void);

    // The number of snapshots taken.
    size_t snapshotCount(void) const { return m_snapshots.size(); }

    // Returns one of the taken snapshots.
    const AQSnapshot& snapshot(size_t idx) const { return *m_snapshots[idx]; }

public:

    // Defines the snapshot statistics.
    enum Statistic
    {
        // The number of snapshots taken.
        StatisticSnapshotsTaken,

        // The total number of records in all snapshots.
        StatisticTotalRecords,

        // The minimum number of records in a snapshot.
        StatisticMinimumRecords,

        // The maximum number of records in a snapshot.
        StatisticMaximumRecords,

        // The total time for all snapshots to be taken.
        StatisticTotalSnapshotTimeMs,

        // The maximum time for any one snapshot cycle.
        StatisticMaxSnapshotTimeMs,

        // The number of statistics.
        StatisticCount,
    };

    // Gets one of the statistic values for this producer.
    unsigned long long getStat(Statistic stat) const { return m_stats[stat]; }

private:

    // Sets a statistic to a fixed value.
    void setStat(Statistic stat, unsigned long long amount) { m_stats[stat] = amount; }

    // Increments one of the statistics for this producer.
    void incrementStat(Statistic stat, unsigned long long amount = 1) { m_stats[stat] += amount; }

    // Updates a statistic if a new value is less than its current value.
    void minimizeStat(Statistic stat, unsigned long long value)
    {
        if (m_stats[stat] > value)
        {
            m_stats[stat] = value;
        }
    }

    // Updates a statistic if a new value is larger than its current value.
    void maximizeStat(Statistic stat, unsigned long long value)
    {
        if (m_stats[stat] < value)
        {
            m_stats[stat] = value;
        }
    }

    // The statistics for this producer.
    unsigned long long m_stats[StatisticCount];

};



#endif
//=============================== End of File ==================================
