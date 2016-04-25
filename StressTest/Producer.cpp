//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Producer.h"

#include "AQWriterItem.h"

#include "TestPointNotifier.h"
#include "Timer.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

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
Producer::Producer(AQReader &reader, int threadNum, void *shm, 
                   size_t shmSize, const vector<size_t> &itemPages, 
                   bool checkLinkId, unsigned int maxOutstanding, 
                   size_t maxPagesPerAppend, TraceManager *trace)
    : m_writer(shm, shmSize, createTrace(trace, threadNum, "p"))
    , m_producerGen(threadNum, m_writer.pageSize(), itemPages)
    , m_maxOutstanding(maxOutstanding)
    , m_items(new AQWriterItem*[maxOutstanding])
    , m_prng(threadNum)
    , m_maxPagesPerAppend(maxPagesPerAppend)
    , m_consumerChannel(reader, m_producerGen, checkLinkId, maxOutstanding, createTrace(trace, threadNum, "c"))
{
    memset(m_stats, 0, sizeof(m_stats));
    for (unsigned int i = 0; i < maxOutstanding; ++i)
    {
        m_items[i] = new AQWriterItem;
    }
}

//------------------------------------------------------------------------------
TraceBuffer *Producer::createTrace(TraceManager *trace, int threadNum, 
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
Producer::~Producer(void)
{
    for (unsigned int i = 0; i < m_maxOutstanding; ++i)
    {
        delete m_items[i];
    }
    delete[] m_items;
    m_items = NULL;
}

//------------------------------------------------------------------------------
const ItemGenerator &Producer::producerGenerator(void)
{
    if (!isJoined())
    {
        throw logic_error("Cannot get the producer for a joined thread");
    }
    return m_producerGen;
}

//------------------------------------------------------------------------------
void Producer::run(void)
{
    for (;;)
    {
        abortIfStop();
        writeItem();
    }
}

//------------------------------------------------------------------------------
void Producer::writeItem(void)
{
    // Select the number of items to make outstanding.
    unsigned int outstanding = m_maxOutstanding;

    // Claim all the item buffers we need.
    size_t pageSize = m_producerGen.pageSize();
    Timer::Ms_t startMs;
    for (unsigned int i = 0; i < outstanding; ++i)
    {
        // Generate the item.
        size_t recSize;
        size_t recPos = 0;
        uint32_t linkId = 0;
        const unsigned char *rec = m_producerGen.get(recSize, linkId);
        size_t initialRecSize = recSize;

        if (m_writer.isExtendable())
        {
            initialRecSize = m_prng.next() % (recSize + 1);
        }

        bool claimed = false;
        for (;;)
        {
            if (i == 0 && !claimed)
            {
                startMs = Timer::start();
            }
            bool claimFailed = true;
            if (!claimed && m_writer.claim(*m_items[i], initialRecSize))
            {
                // Copy in the value for normal mode - but in extendable
                // mode we use append below.
                incrementStat(StatisticClaimSuccess);
                m_items[i]->setLinkIdentifier(linkId);
                if (m_writer.isExtendable())
                {
                    claimed = true;
                }
                else
                {
                    m_producerGen.next();
                    memcpy(&(*m_items[i])[0], rec, recSize);
                    break;
                }
                claimFailed = false;
            }
            if (claimed)
            {
                // Must be in extendable write mode to reach this point.

                // Calculate size to write this time.  If there is less than
                // half a page left, write that half page.  Otherwise write at
                // least half a page.
                size_t appendSize = recSize - recPos;
                if (appendSize > pageSize / 2)
                {
                    size_t maxAppendSize = m_maxPagesPerAppend * pageSize;
                    if (appendSize > maxAppendSize)
                    {
                        appendSize = maxAppendSize;
                    }

                    appendSize = (pageSize / 2) + (m_prng.next() % (appendSize - pageSize / 2));
                }

                // Try to write, tracking successful claim operations based on 
                // whether the list was updated.
                AQWriterItem *last = m_items[i]->last();
                if (m_items[i]->write(&rec[recPos], appendSize))
                {
                    recPos += appendSize;
                    if (last != m_items[i]->last())
                    {
                        incrementStat(StatisticClaimSuccess);
                    }
                    if (recPos == recSize)
                    {
                        m_producerGen.next();
                        break;
                    }
                    claimFailed = false;
                }
            }

            // Sleep to wait for some space to become available.
            if (claimFailed)
            {
                incrementStat(StatisticClaimFailures);
                if ((i > 0 || claimed) && Timer::elapsed(startMs) > 100)
                {
                    // No space available - we should not keep trying to 
                    // allocate because if we did we can end up with an
                    // incomplete item at the head of the queue which 
                    // will impact other producers as well.
                    outstanding = i;

                    if (claimed && m_writer.isExtendable())
                    {
                        // In the extendable case we have not have finished
                        // writing the entire record.  In this case fill
                        // what we can of the last item.
                        outstanding++;
                        AQWriterItem *last = m_items[i]->last();
                        size_t avail = last->capacity() - last->size();
                        if (avail > 0)
                        {
                            m_items[i]->write(&rec[recPos], avail);
                            recPos += avail;
                        }

                        // Set the truncation marker.
                        m_producerGen.next();
                        _snprintf((char *)&(*m_items[i])[0], RECORD_LEN_CHARS + 1, RECORD_LEN_FORMAT, (unsigned int)recPos);
                        (*m_items[i])[RECORD_LEN_CHARS] = ',';
                    }

                    break;
                }
                abortIfStopImmediate();
                yieldMs(0);
            }
        }
    }

    // Now commit all the items in random order.
    if (outstanding > 1)
    {
        shuffleItems(outstanding);
    }
    for (unsigned int i = 0; i < outstanding; ++i)
    {
        m_writer.commit(*m_items[i]);
    }
    Timer::Ms_t durationMs = Timer::elapsed(startMs);

    incrementStat(StatisticTotalCycles);
    incrementStat(StatisticTotalCycleTimeMs, durationMs);
    maximizeStat(StatisticMaxCycleTimeMs, durationMs);
}

//------------------------------------------------------------------------------
void Producer::shuffleItems(unsigned int count)
{
    for (unsigned int i = count - 1; i > 0; i--)
    {
        unsigned int index = m_prng.next() % (i + 1);
        AQWriterItem *temp = m_items[index];
        m_items[index] = m_items[i];
        m_items[i] = temp;
    }
}

#ifdef AQ_TEST_POINT

//------------------------------------------------------------------------------
void Producer::injectTestPointDelay(int tp)
{
    TestPointNotifier *tpn = m_writer.testPointNotifier();

    if (tpn != NULL)
    {
        tpn->registerTestPoint(tp, testPointDelay, &m_writer, (void *)tp);
    }
}

//------------------------------------------------------------------------------
void Producer::testPointDelay(AQ *queue, void *context)
{
    TestPointNotifier *tpn = queue->testPointNotifier();
    if (tpn != NULL)
    {
        tpn->registerTestPoint((int)context, testPointDelay, queue, context);
    }
    Timer::sleep(1);
}

#endif




//=============================== End of File ==================================
