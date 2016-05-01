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

#include <process.h>

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
    : m_hThread(INVALID_HANDLE_VALUE)
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

    if (m_hThread == INVALID_HANDLE_VALUE)
    {
        m_stop = false;
        m_stopImmediate = false;

        unsigned thrdaddr = 0;
        uintptr_t r = _beginthreadex(NULL, 0, threadEntry, this, 0, &thrdaddr);
        if (r == 0)
        {
            unlock();
            throw runtime_error("Cannot create thread");
        }
        m_hThread = (HANDLE)r;
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

    if (m_hThread != INVALID_HANDLE_VALUE)
    {
        m_stop = true;
        unlock();

        DWORD res = WaitForSingleObject(m_hThread, timeoutMs);

        lock();
        if (res == WAIT_OBJECT_0)
        {
            CloseHandle(m_hThread);
            m_hThread = INVALID_HANDLE_VALUE;
        }
    }

    bool joined = m_hThread == INVALID_HANDLE_VALUE;
    unlock();

    return joined;
}

//------------------------------------------------------------------------------
bool WorkerThread::isJoined(void)
{
    lock();
    bool joined = m_hThread == INVALID_HANDLE_VALUE;
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
unsigned __stdcall WorkerThread::threadEntry(void *pt)
{
    try
    {
        ((WorkerThread *)pt)->run();
    }
    catch (const WorkerThreadAbortException&)
    {
    }
    return 0;
}

//------------------------------------------------------------------------------
void WorkerThread::yieldMs(unsigned int ms)
{
    Sleep(ms);
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
    BOOL isWow64 = FALSE;

    if (!IsWow64Process(GetCurrentProcess(), &isWow64))
    {
        isWow64 = FALSE;
    }

    SYSTEM_INFO info;
    if (isWow64)
    {
        GetNativeSystemInfo(&info);
    }
    else
    {
        GetSystemInfo(&info);
    }
    return (int)info.dwNumberOfProcessors;
}




//=============================== End of File ==================================
