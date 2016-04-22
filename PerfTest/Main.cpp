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

#include <iomanip>
#include <iostream>

using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// The number if items in the thread counts array.
#define THREAD_COUNTS                   (sizeof(ThreadCounts) / sizeof(int))

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




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// The thread counts to use.
static const int ThreadCounts[] = { 1 };//, 2, 3, 5, 8, 16 };




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    std::vector<PerfTest *> m_tests;

    // Queue providers.
    AQProvider aqProvider(2, (1 << 20) - 1);
    AQStrawManProvider<CriticalSection> aqReferenceCS(2, (1 << 20) - 1);
    AQStrawManProvider<Mutex> aqReferenceMutex(2, (1 << 20) - 1);

    // Build the list of tests.

    // Build the list of tests.
    //m_tests.push_back(new ThreadOverheadTest(30));
    //m_tests.push_back(NULL);

    for (int i = 0; i < THREAD_COUNTS; ++i)
    {
        m_tests.push_back(new ClaimTest("AQ-Claim", aqProvider, ThreadCounts[i]));
        //m_tests.push_back(new ClaimTest("AQ-Claim[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
        //m_tests.push_back(new ClaimTest("AQ-Claim[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
        //m_tests.push_back(NULL);
    }
    for (int i = 0; i < THREAD_COUNTS; ++i)
    {
        m_tests.push_back(new CommitTest("AQ-Commit", aqProvider, ThreadCounts[i]));
        //m_tests.push_back(new CommitTest("AQ-Commit[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
        //m_tests.push_back(new CommitTest("AQ-Commit[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
        //m_tests.push_back(NULL);
    }
    for (int i = 0; i < THREAD_COUNTS; ++i)
    {
        m_tests.push_back(new ClaimCommitTest("AQ-ClaimCommit", aqProvider, ThreadCounts[i]));
        //m_tests.push_back(new ClaimCommitTest("AQ-ClaimCommit[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
        //m_tests.push_back(new ClaimCommitTest("AQ-ClaimCommit[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
        //m_tests.push_back(NULL);
    }

    m_tests.push_back(new RetrieveTest("AQ-Retrieve", aqProvider));
    //m_tests.push_back(new RetrieveTest("AQ-Retrieve[Ref-CS]", aqReferenceCS));
    //m_tests.push_back(new RetrieveTest("AQ-Retrieve[Ref-Mutex]", aqReferenceMutex));
    //m_tests.push_back(NULL);

    m_tests.push_back(new ReleaseTest("AQ-Release", aqProvider));
    //m_tests.push_back(new ReleaseTest("AQ-Release[Ref-CS]", aqReferenceCS));
    //m_tests.push_back(new ReleaseTest("AQ-Release[Ref-Mutex]", aqReferenceMutex));
    //m_tests.push_back(NULL);

    m_tests.push_back(new RetrieveReleaseTest("AQ-RetrieveRelease", aqProvider));
    //m_tests.push_back(new RetrieveReleaseTest("AQ-RetrieveRelease[Ref-CS]", aqReferenceCS));
    //m_tests.push_back(new RetrieveReleaseTest("AQ-RetrieveRelease[Ref-Mutex]", aqReferenceMutex));
    //m_tests.push_back(NULL);

    for (int i = 0; i < THREAD_COUNTS; ++i)
    {
        m_tests.push_back(new FullQueueTest("AQ-Full", aqProvider, ThreadCounts[i]));
        //m_tests.push_back(new FullQueueTest("AQ-Full[Ref-CS]", aqReferenceCS, ThreadCounts[i]));
        //m_tests.push_back(new FullQueueTest("AQ-Full[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i]));
        //m_tests.push_back(NULL);
    }


    for (int i = 0; i < THREAD_COUNTS; ++i)
    {
        m_tests.push_back(new FullQueueTest("AQ-FullMemCpy", aqProvider, ThreadCounts[i], true));
        //m_tests.push_back(new FullQueueTest("AQ-FullMemCpy[Ref-CS]", aqReferenceCS, ThreadCounts[i], true));
        //m_tests.push_back(new FullQueueTest("AQ-FullMemCpy[Ref-Mutex]", aqReferenceMutex, ThreadCounts[i], true));
        //m_tests.push_back(NULL);
    }

    // Run each test, then publish its results.
    size_t nameWidth = 4;
    for (size_t i = 0; i < m_tests.size(); ++i)
    {
        if (m_tests[i])
        {
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
    
    cout << endl << endl << "*** PRESS ANY KEY" << endl;
    getchar();

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




//=============================== End of File ==================================
