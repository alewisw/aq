#ifndef PRODUCER_H
#define PRODUCER_H
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

#include "ConsumerChannel.h"
#include "ItemGenerator.h"

#include "AQWriter.h"
#include "TraceManager.h"

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

// Encapsulates the logic for a record producer writing into a AQ.
class Producer : public WorkerThread
{
public:

    // Constructs a new producer using the passed shared memory region.  The
    // producer is allocated thread number 'threadNum'.
    //
    // If <trace> is non-NULL a trace buffer is created for the queue.
    Producer(AQReader& reader, int threadNum, void *shm, 
        size_t shmSize, const std::vector<unsigned int> &itemPages, 
        bool checkLinkId, unsigned int maxOutstanding, 
        size_t maxPagesPerAppend, TraceManager *trace);

private:

    // Creates a trace buffer using the passed trace manager and thread
    // number.
    static TraceBuffer *createTrace(TraceManager *trace, int threadNum, 
        const char *prefix);

public:

    // Destroys this thread.
    virtual ~Producer(void);

    // Not implmented - cannot be copied or assigned.
    Producer(const Producer& other);
    Producer& operator=(const Producer& other);

    // Obtains a reference to the producer generator.  This may only
    // be accessed once this thread has been stopped.
    const ItemGenerator &producerGenerator(void);

protected:
    
    // Runs the producer thread.
    virtual void run(void);

private:

    // Writes an item into the queue.
    void writeItem(void);

    // Shuffles the items array up to index 'count' - 1.
    void shuffleItems(unsigned int count);

    // The queue used by this producer.
    AQWriter m_writer;

    // The maximum allowed number of outstanding items.
    unsigned int m_maxOutstanding;

    // The array of items up to m_maxOutstanding.
    AQWriterItem **m_items;

    // The PRNG used for commit ordering.
    Prng m_prng;

    // The number of pages to write per append attempt.
    size_t m_maxPagesPerAppend;

    // The record generator used for producing records.
    ItemGenerator m_producerGen;

public:

    // Defines the producer statistics.
    enum Statistic
    {
        // The number of claim() failures.
        StatisticClaimFailures,

        // The number of claim() successes.
        StatisticClaimSuccess,

        // The total number of write cycles performed.
        StatisticTotalCycles,

        // The total time for all write cycles.
        StatisticTotalCycleTimeMs,

        // The maximum time for any write cycle.
        StatisticMaxCycleTimeMs,

        // The number of statistics.
        StatisticCount,
    };

    // Gets one of the statistic values for this producer.
    unsigned long long getStat(Statistic stat) const { return m_stats[stat]; }

private:

    // Increments one of the statistics for this producer.
    void incrementStat(Statistic stat, unsigned long long amount = 1) { m_stats[stat] += amount; }

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

public:

    // The consumer channel used to compare the output for this producer.
    ConsumerChannel m_consumerChannel;

    // Obtains a reference to the consumer channel.
    ConsumerChannel& consumerChannel(void) { return m_consumerChannel; }

#ifdef AQ_TEST_POINT

public:
    // Causes a delay to be injected at test point 'tp'.
    void injectTestPointDelay(int tp);

    // The actual test point where the delay is inserted.
    static void testPointDelay(AQ *queue, void *context);

#endif

};



#endif
//=============================== End of File ==================================
