#ifndef PERFTEST_H
#define PERFTEST_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Event.h"
#include "WorkerThread.h"

#include "Timer.h"

#include <string>
#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// The default minimum duration in milliseconds.
#define PERF_TEST_DEFAULT_MIN_DURATION_MS   1000




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines a performance test.  The performance test can run in one or more 
// parallel threads.  It runs in all threads and the time between the first
// thread starting an the last thread finishing is the run duration.
//
// The performance test run run multiple times until it completes.
class PerfTest
{
protected:

    // Constructs a new performance test with no threads.
    PerfTest(const std::string& name, aq::Timer::Ms_t minDurationMs = PERF_TEST_DEFAULT_MIN_DURATION_MS);

private:
    // No copy or assignment permitted.
    PerfTest(const PerfTest& other);
    PerfTest& operator=(const PerfTest& other);
public:

    // Destroys this performance test.
    virtual ~PerfTest(void);

private:

    // Defines a Thread class that just calls an executor.
    class Thread
    {
    public:
        Thread(void) { };
        virtual ~Thread(void) { };
        virtual void execute(void) = 0;
    };

    // Encapsulates a callback used to run a particular thread with no arguments.
    template<typename T> class Thread0 : public Thread
    {
    public:
        // Constructs this Thread with a callback and single argument.
        Thread0(T *obj, void (T::*func)(void)) : m_obj(obj), m_func(func) { };
    private:
        // No copy or assignment permitted.
        Thread0<T>(const Thread0<T>& other);
        Thread0<T>& operator=(const Thread0<T>& other);
    public:
        virtual ~Thread0(void) { };
        virtual void execute(void) { ((*m_obj).*(m_func))(); }
    private:
        T *m_obj;
        void (T::*m_func)(void);
    };

    // Encapsulates a callback used to run a particular thread with 1 argument.
    template<typename T, typename U> class Thread1 : public Thread
    {
    public:
        // Constructs this Thread with a callback and single argument.
        Thread1(T *obj, void (T::*func)(U&), U& arg) : m_obj(obj), m_func(func), m_arg(arg) { };
    private:
        // No copy or assignment permitted.
        Thread1<T,U>(const Thread1<T,U>& other);
        Thread1<T,U>& operator=(const Thread1<T,U>& other);
    public:
        virtual ~Thread1(void) { };
        virtual void execute(void) { ((*m_obj).*(m_func))(m_arg); }
    private:
        T *m_obj;
        void (T::*m_func)(U&);
        U& m_arg;
    };

    // The thread that runs the performance tests.
    class PerfThread : public WorkerThread
    {
    public:
        // Constructs this thread, setting it to run on 'test' with thread
        // number 'threadNum'.
        PerfThread(PerfTest& test, PerfTest::Thread *executor, size_t threadNum)
            : m_test(test)
            , m_executor(executor)
            , m_threadNum(threadNum)
        {
        }

    private:
        // No copy or assignment permitted.
        PerfThread(const PerfThread& other);
        PerfThread& operator=(const PerfThread& other);
    public:

        // Destroys this thread.
        virtual ~PerfThread(void) { delete m_executor; };

    private:

        // The test object.
        PerfTest& m_test;

        // The executor for this thread.
        PerfTest::Thread *m_executor;

        // The thread number.
        size_t m_threadNum;

        // The event used to wake this thread.
        Event m_wakeEvent;

        // The event signalled by this thread when it finishes an iteration.
        Event m_finishEvent;

    public:

        // Returns the executor of this thread.
        PerfTest::Thread *exector(void) const { return m_executor;  }

        // The event that wakes this thread.
        Event& wakeEvent(void) { return m_wakeEvent;  }

        // The event that signals that this thread has finished.
        Event& finishEvent(void) { return m_finishEvent; }

    protected:

        // Runs this thread.  Override in sub-classes to perform thread operations.
        virtual void run(void) { m_test.runThread(m_threadNum); }
        
    };

private:

    // Adds a thread to this performance test.
    void addExecutor(Thread *executor);

protected:

    // Adds a function called in a thread with no arguments.
    template<typename X> void addThread(void (X::*func)(void)) 
    { 
        addExecutor(new Thread0<X>((X *)this, func));  
    };

    // Adds a function called in a thread with a single argument.
    template<typename X, typename Y> void addThread(void (X::*func)(Y& arg), Y& arg) 
    { 
        addExecutor(new Thread1<X, Y>((X *)this, func, arg)); 
    };

private:

    // The name of this test.
    std::string m_name;

    // The mininmum run-time in milliseconds.
    aq::Timer::Ms_t m_minDurationMs;

    // The set of threads.
    std::vector<PerfThread *> m_threads;

    // The lock that protects access to the statistics variables during 
    // thread execution.
    Mutex m_lock;

    // Set to true if the first thread entry has been measured.
    bool m_firstMeasured;

    // The time when the first thread entered.
    aq::Timer::Ms_t m_firstThreadEntryMs;

    // The time when the last thread exited.
    aq::Timer::Ms_t m_lastThreadExitMs;

    // The number of iterations performed.
    unsigned long m_iterationCount;

    // The total duration from all iterations.
    aq::Timer::Ms_t m_totalDurationMs;

    // Called in the thread 'threadNum' to run the test for that thread.
    void runThread(size_t threadNum);

public:

    // Sets the test duration in milliseconds.
    void setDurationMs(aq::Timer::Ms_t ms) { m_minDurationMs = ms; }

    // Runs this test.
    void run(void);

protected:

    // Called before the test is run to setup the test in the sub-class.
    virtual void before(void) { }

    // Called before each iteration of the test.
    virtual void beforeIteration(void) { }

    // Called after the test is run to clean-up the test in the sub-class.
    virtual void after(void) { }

public:

    // Gets the name of this performance test.
    const std::string& name(void) const { return m_name; }

    // Gets the thread count for this test.
    size_t threadCount(void) const { return m_threads.size(); }

    // Gets the number of iterations performed for this test.
    unsigned long iterationCount(void) const { return m_iterationCount; }

    // The total number of operations that were performed.
    virtual unsigned long totalOperationCount(void) const { return m_iterationCount; }

    // Gets the total duration of all runs in this test.
    aq::Timer::Ms_t totalDurationMs(void) const { return m_totalDurationMs; }

    // Gets a description of the configuration of this test.
    virtual std::string config(void) const;

    // Gets a description of the results for this test.
    virtual std::string results(void) const;

};



#endif
//=============================== End of File ==================================
