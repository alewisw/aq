//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"

#include "ClaimTest.h"
#include "ClaimCommitTest.h"
#include "CommitTest.h"
#include "FullQueueTest.h"
#include "ReleaseTest.h"
#include "RetrieveReleaseTest.h"
#include "RetrieveTest.h"
#include "AQStrawManProvider.h"
#include "AQProvider.h"
#include "ThreadOverheadTest.h"

#include "Optarg.h"

#include <iomanip>
#include <iostream>

using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// We run for this many seconds in each produce/consume window.
#define DEFAULT_TEST_DURATION_SECS      5

// The default set of pages to allocate.
#define DEFAULT_THREAD_COUNTS           {1, 2, 3}

// Default enable option for the straw-man queue with a Mutex used for concurrency protection.
#define DEFAULT_STRAW_MUTEX             false

// Default enable option for the straw-man queue with a CriticalSection used for concurrency protection.
#define DEFAULT_STRAW_CRITSEC           false

// The widths of each column.
#define THREAD_COUNT_WIDTH              3
#define CONFIG_WIDTH                    24
#define ITERATION_COUNT_WIDTH           5
#define DURATION_MS_WIDTH               9
#define OPERATION_COUNT_WIDTH           9
#define OPERATIONS_PER_SEC_WIDTH        9
#define RESULTS_WIDTH                   24




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------

/*
Name                    |Th#|Configuration           |#Iter|  ms/Iter|     #Ops|  Ops/sec|Results                 |
AQ-FullMemCpy           |  2|1048575 pages @ 4 bytes |    5|  253.000|  5242870|  4144561|contention[0]           |
AQ-FullMemCpy[Ref-CS]   |  2|1048575 pages @ 4 bytes |    3|  343.333|  3145725|  3054102|                        |
AQ-FullMemCpy[Ref-Mutex]|  2|1048575 pages @ 4 bytes |    1| 4493.000|  1048575|   233380|                        |

AQ-FullMemCpy           |  3|1048575 pages @ 4 bytes |    4|  327.500|  4194296|  3201753|contention[765091]      |
AQ-FullMemCpy[Ref-CS]   |  3|1048575 pages @ 4 bytes |    5|  246.400|  5242870|  4255576|                        |
AQ-FullMemCpy[Ref-Mutex]|  3|1048575 pages @ 4 bytes |    1|18237.000|  1048574|    57497|                        |

AQ-FullMemCpy           |  4|1048575 pages @ 4 bytes |    3|  379.333|  3145716|  2764250|contention[1588263]     |
AQ-FullMemCpy[Ref-CS]   |  4|1048575 pages @ 4 bytes |    5|  234.200|  5242875|  4477263|                        |
AQ-FullMemCpy[Ref-Mutex]|  4|1048575 pages @ 4 bytes |    1| 4259.000|  1048575|   246202|                        |

AQ-FullMemCpy           |  6|1048575 pages @ 4 bytes |    3|  436.667|  3145710|  2401305|contention[2744364]     |
AQ-FullMemCpy[Ref-CS]   |  6|1048575 pages @ 4 bytes |    4|  292.750|  4194300|  3581810|                        |
AQ-FullMemCpy[Ref-Mutex]|  6|1048575 pages @ 4 bytes |    1| 3416.000|  1048575|   306960|                        |

AQ-FullMemCpy           |  9|1048575 pages @ 4 bytes |    2|  538.500|  2097136|  1947201|contention[5099088]     |
AQ-FullMemCpy[Ref-CS]   |  9|1048575 pages @ 4 bytes |    4|  288.750|  4194272|  3631404|                        |
AQ-FullMemCpy[Ref-Mutex]|  9|1048575 pages @ 4 bytes |    1| 3354.000|  1048568|   312632|                        |

AQ-FullMemCpy           | 17|1048575 pages @ 4 bytes |    2|  562.000|  2097120|  1865765|contention[5380773]     |
AQ-FullMemCpy[Ref-CS]   | 17|1048575 pages @ 4 bytes |    4|  288.750|  4194240|  3631377|                        |
AQ-FullMemCpy[Ref-Mutex]| 17|1048575 pages @ 4 bytes |    1| 3354.000|  1048560|   312630|                        |
*/


//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Prints the result header.
static void printTestHeader(size_t nameWidth);

// Prints the test results.
static void printTestConfig(size_t nameWidth, PerfTest& test);

// Prints the test results.
static void printTestResults(PerfTest& test);

// Configures the performance test run using the passed options.
static void configure(Optarg &cfg);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// The test duration, in seconds.
static uint32_t TestDurationSecs = DEFAULT_TEST_DURATION_SECS;

// The thread counts to use.
static std::vector<unsigned int> ThreadCounts;

// If true, enable the Mutex-based Straw Man for performance comparison.
static bool StrawMutex = DEFAULT_STRAW_MUTEX;

// If true, enable the CriticalSection-based Straw Man for performance comparison.
static bool StrawCritSec = DEFAULT_STRAW_CRITSEC;

// The thread overhead test thread count, 0 to disable.
static uint32_t ThreadOverheadThreadCount = 0;

// The tests to execute.
static bool TestClaim = false;
static bool TestCommit = false;
static bool TestClaimCommit = false;
static bool TestRetrieve = false;
static bool TestRelease = false;
static bool TestRetrieveRelease = false;
static bool TestFull = false;
static bool TestFullMemcpy = false;



//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    const int defaultThreadCounts[] = DEFAULT_THREAD_COUNTS;
    for (size_t i = 0; i < sizeof(defaultThreadCounts) / sizeof(defaultThreadCounts[0]); ++i)
    {
        ThreadCounts.push_back(defaultThreadCounts[i]);
    }
    Optarg opt(argc, argv);
    configure(opt);
    int nProcessors = WorkerThread::numberOfProcessors();
    cout << endl << endl << "Running Performance Test: ";
    if (nProcessors == 1)
    {
        cout << "1 processor available";
    }
    else if (nProcessors > 1)
    {
        cout << nProcessors << " processors available";
    }
    cout << endl;


    std::vector<PerfTest *> m_tests;

    // Queue providers.
    AQProvider aqProvider(2, (1 << 20) - 1);
    AQStrawManProvider<CriticalSection> aqReferenceCS(2, (1 << 20) - 1);
    AQStrawManProvider<Mutex> aqReferenceMutex(2, (1 << 20) - 1);

    // Build the list of tests.
    if (ThreadOverheadThreadCount > 0)
    {
        m_tests.push_back(new ThreadOverheadTest(ThreadOverheadThreadCount));
        m_tests.push_back(NULL);
    }

    if (TestClaim)
    {
        for (size_t i = 0; i < ThreadCounts.size(); ++i)
        {
            m_tests.push_back(new ClaimTest("AQ-Claim", aqProvider, ThreadCounts[i]));
            if (StrawCritSec)
            {
                m_tests.push_back(new ClaimTest("AQ-Claim[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
            }
            if (StrawMutex)
            {
                m_tests.push_back(new ClaimTest("AQ-Claim[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
            }
            if (StrawCritSec || StrawMutex)
            {
                m_tests.push_back(NULL);
            }
        }
    }
    if (TestCommit)
    {
        for (size_t i = 0; i < ThreadCounts.size(); ++i)
        {
            m_tests.push_back(new CommitTest("AQ-Commit", aqProvider, ThreadCounts[i]));
            if (StrawCritSec)
            {
                m_tests.push_back(new CommitTest("AQ-Commit[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
            }
            if (StrawMutex)
            {
                m_tests.push_back(new CommitTest("AQ-Commit[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
            }
            if (StrawCritSec || StrawMutex)
            {
                m_tests.push_back(NULL);
            }
        }
    }
    if (TestClaimCommit)
    {
        for (size_t i = 0; i < ThreadCounts.size(); ++i)
        {
            m_tests.push_back(new ClaimCommitTest("AQ-ClaimCommit", aqProvider, ThreadCounts[i]));
            if (StrawCritSec)
            {
                m_tests.push_back(new ClaimCommitTest("AQ-ClaimCommit[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
            }
            if (StrawMutex)
            {
                m_tests.push_back(new ClaimCommitTest("AQ-ClaimCommit[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
            }
            if (StrawCritSec || StrawMutex)
            {
                m_tests.push_back(NULL);
            }
        }
    }

    if (TestRetrieve)
    {
        m_tests.push_back(new RetrieveTest("AQ-Retrieve", aqProvider));
        if (StrawCritSec)
        {
            m_tests.push_back(new RetrieveTest("AQ-Retrieve[Ref-CS]", aqReferenceCS));
        }
        if (StrawMutex)
        {
            m_tests.push_back(new RetrieveTest("AQ-Retrieve[Ref-Mutex]", aqReferenceMutex));
        }
        if (StrawCritSec || StrawMutex)
        {
            m_tests.push_back(NULL);
        }
    }

    if (TestRelease)
    {
        m_tests.push_back(new ReleaseTest("AQ-Release", aqProvider));
        if (StrawCritSec)
        {
            m_tests.push_back(new ReleaseTest("AQ-Release[Ref-CS]", aqReferenceCS));
        }
        if (StrawMutex)
        {
            m_tests.push_back(new ReleaseTest("AQ-Release[Ref-Mutex]", aqReferenceMutex));
        }
        if (StrawCritSec || StrawMutex)
        {
            m_tests.push_back(NULL);
        }
    }

    if (TestRetrieveRelease)
    {
        m_tests.push_back(new RetrieveReleaseTest("AQ-RetrieveRelease", aqProvider));
        if (StrawCritSec)
        {
            m_tests.push_back(new RetrieveReleaseTest("AQ-RetrieveRelease[Ref-CS]", aqReferenceCS));
        }
        if (StrawMutex)
        {
            m_tests.push_back(new RetrieveReleaseTest("AQ-RetrieveRelease[Ref-Mutex]", aqReferenceMutex));
        }
        if (StrawCritSec || StrawMutex)
        {
            m_tests.push_back(NULL);
        }
    }

    if (TestFull)
    {
        for (size_t i = 0; i < ThreadCounts.size(); ++i)
        {
            m_tests.push_back(new FullQueueTest("AQ-Full", aqProvider, ThreadCounts[i]));
            if (StrawCritSec)
            {
                m_tests.push_back(new FullQueueTest("AQ-Full[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
            }
            if (StrawMutex)
            {
                m_tests.push_back(new FullQueueTest("AQ-Full[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
            }
            if (StrawCritSec || StrawMutex)
            {
                m_tests.push_back(NULL);
            }
        }
    }

    if (TestFullMemcpy)
    {
        for (size_t i = 0; i < ThreadCounts.size(); ++i)
        {
            m_tests.push_back(new FullQueueTest("AQ-FullMemCpy", aqProvider, ThreadCounts[i], true));
            if (StrawCritSec)
            {
                m_tests.push_back(new FullQueueTest("AQ-FullMemCpy[Ref-CS]", aqReferenceCS, ThreadCounts[i], true));
            }
            if (StrawMutex)
            {
                m_tests.push_back(new FullQueueTest("AQ-FullMemCpy[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i], true));
            }
            if (StrawCritSec || StrawMutex)
            {
                m_tests.push_back(NULL);
            }
        }
    }

    // Run each test, then publish its results.
    size_t nameWidth = 4;
    for (size_t i = 0; i < m_tests.size(); ++i)
    {
        if (m_tests[i])
        {
            m_tests[i]->setDurationMs(TestDurationSecs * 1000);
            size_t width = m_tests[i]->name().size();
            if (width > nameWidth)
            {
                nameWidth = width;
            }
        }
    }
    printTestHeader(nameWidth);
    for (size_t i = 0; i < m_tests.size(); ++i)
    {
        if (m_tests[i] == NULL)
        {
            cout << endl;
        }
        else
        {
            printTestConfig(nameWidth, *m_tests[i]);
            m_tests[i]->run();
            printTestResults(*m_tests[i]);
        }
    }

    for (size_t i = 0; i < m_tests.size(); ++i)
    {
        if (m_tests[i])
        {
            delete m_tests[i];
        }
    }
    m_tests.clear();

    return 0;
}

//------------------------------------------------------------------------------
static void printTestHeader(size_t nameWidth)
{
    cout << left << setw(1) << "|"
        << setw(nameWidth) << "Name" << setw(1) << "|"
        << setw(THREAD_COUNT_WIDTH) << "Th#" << setw(1) << "|"
        << setw(CONFIG_WIDTH) << "Configuration" << setw(1) << "|"
        << right << setw(ITERATION_COUNT_WIDTH) << "#Iter" << setw(1) << "|"
        << right << setw(DURATION_MS_WIDTH) << "ms/Iter" << setw(1) << "|"
        << right << setw(OPERATION_COUNT_WIDTH) << "#Ops" << setw(1) << "|"
        << right << setw(OPERATIONS_PER_SEC_WIDTH) << "Ops/sec" << setw(1) << "|"
        << left << setw(RESULTS_WIDTH) << "Results" << setw(1) << "|" << endl;
}

//------------------------------------------------------------------------------
static void printTestConfig(size_t nameWidth, PerfTest& test)
{
    cout << left << setw(1) << "|"
        << setw(nameWidth) << test.name() << setw(1) << "|"
        << right << setw(THREAD_COUNT_WIDTH) << test.threadCount() << setw(1) << "|"
        << left << setw(CONFIG_WIDTH) << test.config() << setw(1) << "|";
}

//------------------------------------------------------------------------------
static void printTestResults(PerfTest& test)
{
    double ms = (double)test.totalDurationMs();

    double durationMs = ms / (double)test.iterationCount();
    double opsPerSec = (double)test.totalOperationCount() / (ms / 1000.0);
    cout << right << setw(ITERATION_COUNT_WIDTH) << test.iterationCount() << setw(1) << "|"
         << right << setw(DURATION_MS_WIDTH) << fixed << setprecision(3) << durationMs << setw(1) << "|"
         << right << setw(OPERATION_COUNT_WIDTH) << test.totalOperationCount() << setw(1) << "|"
         << right << setw(OPERATIONS_PER_SEC_WIDTH) << fixed << setprecision(0) << opsPerSec << setw(1) << "|"
         << left << setw(RESULTS_WIDTH) << test.results() << setw(1) << "|" << endl;
}

//------------------------------------------------------------------------------
static void configure(Optarg &cfg)
{
    cfg.opt('d', TestDurationSecs, "The minimum duration of each test execution in seconds.");
    cfg.opt('t', ThreadCounts, "A comma-separated list of thread counts; that is the number of concurrent producers threads to use for each test case.  Test cases are run separatly for each entry in the list.");
    cfg.opt('c', StrawCritSec, "Enables a straw-man comparison queue that uses critical sections for concurrency protection.");
    cfg.opt('m', StrawMutex, "Enables a straw-man comparison queue that uses mutexes for concurrency protection.");
    cfg.opt('T', ThreadOverheadThreadCount, "Enables the thread overhead test with a configured number of threads or 0 to disable the test.");

    cfg.opt('A', TestClaim, "Enables the AQWriter::claim() test.");
    cfg.opt('O', TestCommit, "Enables the AQWriter::commit() test.");
    cfg.opt('C', TestClaimCommit, "Enables the AQWriter::claim() followed by AQWriter::commit() combination test.");
    cfg.opt('E', TestRetrieve, "Enables the AQReader::retrieve() test.");
    cfg.opt('L', TestRelease, "Enables the AQReader::release() test.");
    cfg.opt('R', TestRetrieveRelease, "Enables the AQReader::retrieve() followed by AQReader::release() combination test.");
    cfg.opt('F', TestFull, "Enables the full multi-producer / single consumer queue test.");
    cfg.opt('M', TestFullMemcpy, "Enables the full multi-producer / single consumer queue test with additional memcpy() over all data regions.");

    if (cfg.hasOpt('h', "Show the command line option help."))
    {
        cout << endl << cfg.helpMessage() << endl;
        exit(0);
    }
}




//=============================== End of File ==================================
