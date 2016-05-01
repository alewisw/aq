#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Mutex.h"

#include <pthread.h>




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

// Encapsulates thread execution for a producer or snapshot worker.
class WorkerThread
{
protected:

    // Constructs new worker thread.
    WorkerThread(void);

public:

    // Destroys this thread.
    virtual ~WorkerThread(void);

    // Not implmented - cannot be copied or assigned.
    WorkerThread(const WorkerThread& other);
    WorkerThread& operator=(const WorkerThread& other);

    // Starts this thread of execution.
    void start(void);

    // Notifies this thread that it is to stop executing.
    void stop(void);

    // Notifies this thread that it is to stop executing immediatly
    // regardless of where it is waiting or what it is doing.
    void stopImmediate(void);

    // Blocks until this thread has stopped execution or the passed
    // timeout is reached.  Returns true if the thread is stopped, 
    // or false if the timeout was reached.
    bool join(unsigned int timeoutMs);

    // Returns true if this thread has been joined.
    bool isJoined(void);

    // Yields the running thread for 'ms' milliseconds.
    static void yieldMs(unsigned int ms);

protected:

    // Locks this thread's mutex.
    void lock(void);

    // Unlocks this thread's mutex.
    void unlock(void);

    // Runs this thread.  Override in sub-classes to perform thread operations.
    virtual void run(void) = 0;

public:

    // Aborts this thread if it has been stopped.  Only call from within run().
    void abortIfStop(void);

    // Aborts this thread if it has been stopped with immediate effect. 
    // Only call from within run().
    void abortIfStopImmediate(void);

private:

    // The thread entry-point function where 'pt' is the WorkerThread object.
    static void *threadEntry(void *pt);

    // Set to true if this thread has started.
    bool m_started;
    
    // The handle for this thread.
    pthread_t m_thread;
    
    // Set to true if the thread is to stop; protected by m_lock.
    bool m_stop;
    bool m_stopImmediate;

    // Protects m_stop for this thread.  Can also be used by sub-classes to protect
    // their internal data by using the lock() and unlock() methods.
    Mutex m_lock;

public:

    // Gets the number of processors available on this machine or -1 if the 
    // number of processors could not be determined.
    static int numberOfProcessors(void);

};



#endif
//=============================== End of File ==================================
