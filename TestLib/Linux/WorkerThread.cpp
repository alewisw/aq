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

#include <stdexcept>
#include <time.h>

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

// This is a placeholder for a processor thread aborting execution.
class WorkerThreadAbortException : public exception { };




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
WorkerThread::WorkerThread(void)
    : m_started(false)
    , m_stop(false)
    , m_stopImmediate(false)
{
}

//------------------------------------------------------------------------------
WorkerThread::~WorkerThread(void)
{
}

//------------------------------------------------------------------------------
void WorkerThread::start(void)
{
    lock();

    if (!m_started)
    {
        m_stop = false;
        m_stopImmediate = false;
        
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
        int res = pthread_create(&m_thread, &tattr, threadEntry, this);
        pthread_attr_destroy(&tattr);
        
        if (res != 0)
        {
            unlock();
            throw runtime_error("Cannot create thread");
        }
        m_started = true;
    }

    unlock();
}

//------------------------------------------------------------------------------
void WorkerThread::stop(void)
{
    lock();
    m_stop = true;
    unlock();
}

//------------------------------------------------------------------------------
void WorkerThread::stopImmediate(void)
{
    lock();
    m_stop = true;
    m_stopImmediate = true;
    unlock();
}

//------------------------------------------------------------------------------
bool WorkerThread::join(unsigned int timeoutMs)
{
    lock();

    if (m_started)
    {
        m_stop = true;
        unlock();
        
        struct timespec when;
        clock_gettime(CLOCK_REALTIME, &when);
        when.tv_sec += (timeoutMs + 999) / 1000;
        
        void *p = NULL;
        int res = pthread_timedjoin_np(m_thread, &p, &when);

        lock();
        if (res == 0)
        {
            m_started = false;
        }
    }
    bool joined = !m_started;
    unlock();

    return joined;
}

//------------------------------------------------------------------------------
bool WorkerThread::isJoined(void)
{
    lock();
    bool joined = !m_started;
    unlock();

    return joined;
}

//------------------------------------------------------------------------------
void WorkerThread::lock(void)
{
    m_lock.lock();
}

//------------------------------------------------------------------------------
void WorkerThread::unlock(void)
{
    m_lock.unlock();
}

//------------------------------------------------------------------------------
void *WorkerThread::threadEntry(void *pt)
{
    try
    {
        ((WorkerThread *)pt)->run();
    }
    catch (const WorkerThreadAbortException&)
    {
    }
    return NULL;
}

//------------------------------------------------------------------------------
void WorkerThread::yieldMs(unsigned int ms)
{
    struct timespec ts;
    
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

//------------------------------------------------------------------------------
void WorkerThread::abortIfStop(void)
{
    lock();
    bool stop = m_stop;
    unlock();

    if (stop)
    {
        throw WorkerThreadAbortException();
    }
}

//------------------------------------------------------------------------------
void WorkerThread::abortIfStopImmediate(void)
{
    lock();
    bool stop = m_stopImmediate;
    unlock();

    if (stop)
    {
        throw WorkerThreadAbortException();
    }
}

//------------------------------------------------------------------------------
int WorkerThread::numberOfProcessors(void)
{
    return -1;
}




//=============================== End of File ==================================
