//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "PerfTest.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace aq;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// No longer than two minutes to run a test.
#define PERF_TEST_TIMEOUT_MS            (2 * 60 * 1000)




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
PerfTest::PerfTest(const std::string& name, Timer::Ms_t minDurationMs) 
    : m_name(name)
    , m_minDurationMs(minDurationMs)
{
}

//------------------------------------------------------------------------------
PerfTest::~PerfTest(void)
{
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        delete m_threads[i];
    }
}

//------------------------------------------------------------------------------
void PerfTest::addExecutor(Thread *executor)
{
    m_threads.push_back(new PerfThread(*this, executor, m_threads.size()));
}

//------------------------------------------------------------------------------
void PerfTest::runThread(size_t threadNum)
{
    PerfThread *thread = m_threads[threadNum];

    for (;;)
    {
        thread->finishEvent().set();
        thread->wakeEvent().block();
        thread->wakeEvent().reset();
        thread->abortIfStop();

        m_lock.lock();
        if (!m_firstMeasured)
        {
            m_firstThreadEntryMs = Timer::start();
            m_firstMeasured = true;
        }
        m_lock.unlock();

        thread->exector()->execute();

        m_lock.lock();
        m_lastThreadExitMs = Timer::start();
        m_lock.unlock();

    }
}

//------------------------------------------------------------------------------
void PerfTest::run(void)
{
    before();

    // Start all threads and wait for them to be ready to run.
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        m_threads[i]->finishEvent().reset();
        m_threads[i]->start();
    }
    m_iterationCount = 0;
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        m_threads[i]->finishEvent().block();
    }

    m_totalDurationMs = 0;
    do
    {
        m_lock.lock();
        m_firstMeasured = false;
        m_lock.unlock();

        for (size_t i = 0; i < m_threads.size(); ++i)
        {
            m_threads[i]->finishEvent().reset();
        }
        beforeIteration();
        for (size_t i = 0; i < m_threads.size(); ++i)
        {
            m_threads[i]->wakeEvent().set();
        }
        m_iterationCount++;
        for (size_t i = 0; i < m_threads.size(); ++i)
        {
            m_threads[i]->finishEvent().block();
        }

        m_lock.lock();
        m_totalDurationMs += m_lastThreadExitMs - m_firstThreadEntryMs;
        m_lock.unlock();
    } while (m_totalDurationMs < m_minDurationMs);

    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        m_threads[i]->stopImmediate();
        m_threads[i]->wakeEvent().set();
    }
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        if (!m_threads[i]->join(PERF_TEST_TIMEOUT_MS))
        {
            cerr << "Aborted performance test due to test taking longer than " 
                  << PERF_TEST_TIMEOUT_MS << " ms to complete";
            abort();
        }
    }

    after();
}

//------------------------------------------------------------------------------
string PerfTest::config(void) const
{
    return string("");
}

//------------------------------------------------------------------------------
string PerfTest::results(void) const
{
    return string("");
}




//=============================== End of File ==================================
