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

#include "Producer.h"
#include "SnapshotTaker.h"
#include "SnapshotValidator.h"

#include "AQReader.h"
#include "AQExternMemory.h"

#include "CtrlOverlay.h"
#include "Timer.h"
#include "TraceManager.h"

#include "Optarg.h"

#include <string.h>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;



//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// We run for this many seconds in each produce/consume window.
#define DEFAULT_RUN_TIME_SECS           5

// We run this many loops by default; 0 for infinite.
#define DEFAULT_RUN_LOOPS               0

// The default number of producers in the test.
#define DEFAULT_PRODUCER_COUNT          3

// The default number of threads to start which just take snapshots.
#define DEFAULT_SNAPSHOT_TAKER_COUNT    1

// The default shared memory size in bytes.
#define DEFAULT_SHM_SIZE                894007

// The default page size shift.
#define DEFAULT_PAGE_SIZE_SHIFT         5

// The default set of pages to allocate.
#define DEFAULT_PAGE_COUNT_ALLOC        {3, 4, 5}

// Producers never take more than this long to commit a record.
#define DEFAULT_COMMIT_TIMEOUT_MS       1000

// The default formatting options.
#define DEFAULT_FORMAT_OPTIONS          (0)//(AQ::OPTION_LINK_IDENTIFIER)//(AQ::OPTION_CRC32)// | AQ::OPTION_LINK_IDENTIFIER)// | AQ::OPTION_EXTENDABLE)

// The default maximum number of outstanding records for each producer.
#define DEFAULT_MAX_OUTSTANDING         30

// The default maximum number of pages per append operation in extendable mode.
#define DEFAULT_MAX_PAGES_PER_APPEND    4

// The default maximum time between snapshots being taken.
#define DEFAULT_MAX_SNAPSHOT_PERIOD_MS  1000

// The default for tracing enable/disable.
#define DEFAULT_TRACE_ENABLE_PRODUCER   false
#define DEFAULT_TRACE_ENABLE_CONSUMER   false

// The default delay introduction settings; set to true to introduce delays.
#define DEFAULT_TPDELAY_CLAIM_BEFORE_WRITE_HEAD_REF         false
#define DEFAULT_TPDELAY_CLAIM_BEFORE_WRITE_CTRL_SKIP_PAGES  false
#define DEFAULT_TPDELAY_CLAIM_BEFORE_WRITE_CTRL             false

// The number of bytes to allocate to shared memory guard regions.
#define SHM_GUARD_SIZE                  8

// The guard byte placed in the shared memory region.
#define SHM_GUARD_BYTE                  0xCD

// The maximum number of records to consume in a loop before checking the 
// system clock.
#define CONSUME_LOOP_COUNTER            1000



//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------

// The states for the produce/consume state machine.
enum ProduceConsumeState
{
    // Currently running.
    ProduceConsumeRunning,

    // All producers have been instructed to stop.
    ProduceConsumeStopping,

    // All producers have joined.
    ProduceConsumeJoined,

    // All producers have joined and the reader is empty.
    ProduceConsumeComplete,
};




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Runs the main produce/consume logic.
static void produceConsume(AQReader &reader);

// Prints the statistics for the loop 'loop' given the passed set of producers.
static void printStatistics(int loop);

// Asserts that the passed record 'item' is valid for the set of producers 
// 'producers'.
static void assertRecord(AQItem *item);

// Logs an assertion failure to the passed output stream.
static void logAssertFailed(ostream& out, const string &msg, TraceManager *tm);

// Allocates a new reader item for processing.
static AQItem *allocReaderItem(void);

// Configures the stress test run using the passed options.
static void configure(Optarg &cfg);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// The run time for each produce/consume cycle.
static uint32_t RunTimeSecs = DEFAULT_RUN_TIME_SECS;

// The number of loops through the produce/consume cycle.
static size_t RunLoops = DEFAULT_RUN_LOOPS;

// The number of producers to run.
static int ProducerCount = DEFAULT_PRODUCER_COUNT;

// The number of snapshot threads to run.
static int SnapshotTakerCount = DEFAULT_SNAPSHOT_TAKER_COUNT;

// The shared memory size.
static size_t ShmSize = DEFAULT_SHM_SIZE;

// The page size shift.
static int PageSizeShift = 5;

// The page sizes to allocate to test runs.
static std::vector<unsigned int> PageSizeAlloc;

// The maximum time for a produce to spend waiting to commit.
static unsigned int CommitTimeoutMs = DEFAULT_COMMIT_TIMEOUT_MS;

// The default set of formatting options.
static unsigned int FormatOptions = DEFAULT_FORMAT_OPTIONS;

// The maximum number of outstanding records for each producer.
static unsigned int MaxOutstanding = DEFAULT_MAX_OUTSTANDING;

// The maximum number of pages to write per append operation in extendable mode.
static size_t MaxPagesPerAppend = DEFAULT_MAX_PAGES_PER_APPEND;

// The maximum period between snapshots being taken.
static unsigned int MaxSnapshotPeriodMs = DEFAULT_MAX_SNAPSHOT_PERIOD_MS;

// Set to true to enable tracing.
static bool TraceEnableConsumer = DEFAULT_TRACE_ENABLE_CONSUMER;
static bool TraceEnableProducer = DEFAULT_TRACE_ENABLE_PRODUCER;

#ifdef AQ_TEST_POINT
static bool TpDelayClaimBeforeWriteHeadRef = DEFAULT_TPDELAY_CLAIM_BEFORE_WRITE_HEAD_REF;
static bool TpDelayClaimBeforeWriteCtrlSkipPages = DEFAULT_TPDELAY_CLAIM_BEFORE_WRITE_CTRL_SKIP_PAGES;
static bool TpDelayClaimBeforeWriteCtrl = DEFAULT_TPDELAY_CLAIM_BEFORE_WRITE_CTRL;
#endif

// The shared memory.
static unsigned char *Shm = NULL;

// The trace manager.
static TraceManager *Trace = NULL;

// The collection of producers.
static vector<Producer *> Producers;

// The collection of snapshot takers.
static vector<SnapshotTaker *> SnapshotTakers;

// The retrieve success and failure counters.
static unsigned long long StatRetreiveSuccess = 0;
static unsigned long long StatRetreiveFailure = 0;
static unsigned long long StatRecordComplete = 0;
static unsigned long long StatRecordIncomplete = 0;
static Timer::Ms_t StatMaxConsumePeriodMs = 0;

// The circular queue of received records.
static SnapshotValidator *SnapValidator;

// The reader items that are currently free for use
static vector<AQItem *> FreeReaderItems;




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Fill in the defaults for the page size allocation vector.
    static const size_t pageSizeAllocDefault[] = DEFAULT_PAGE_COUNT_ALLOC;
    for (size_t i = 0; i < sizeof(pageSizeAllocDefault)
        / sizeof(pageSizeAllocDefault[0]); ++i)
    {
        PageSizeAlloc.push_back(pageSizeAllocDefault[i]);
    }

    // Configures the test case.
    Optarg optarg(argc, argv);
    configure(optarg);
    int nProcessors = WorkerThread::numberOfProcessors();
    cout << endl << endl << "Running Stress Test: ";
    if (nProcessors == 1)
    {
        cout << "1 processor available";
    }
    else if (nProcessors > 1)
    {
        cout << nProcessors << " processors available";
    }
    cout << endl;


    // Create the shared memory region and initialize.
    Shm = new unsigned char[SHM_GUARD_SIZE + ShmSize + SHM_GUARD_SIZE];
    memset(Shm, SHM_GUARD_BYTE, SHM_GUARD_SIZE + ShmSize + SHM_GUARD_SIZE);
    assertShmGuard();

    // Allocate a base amount of reader items.
    for (size_t i = 0; i < MaxOutstanding * ProducerCount; ++i)
    {
        FreeReaderItems.push_back(new AQItem);
    }

    // Create the trace manager if required.
    Trace = (TraceEnableConsumer || TraceEnableProducer) ? new TraceManager(TraceManager::String, 1000) : NULL;

    AQExternMemory sm(&Shm[SHM_GUARD_SIZE], ShmSize);

    // Create the reader and format the memory.
    AQReader consumer(sm,
        TraceEnableConsumer ? Trace->createBuffer("con" /*, 1000000*/) : NULL);
    consumer.format(PageSizeShift, CommitTimeoutMs, FormatOptions);
    SnapValidator = new SnapshotValidator(consumer, MaxOutstanding);
    assertShmGuard();

    // Create and start the producers.
    bool checkLinkId = (FormatOptions & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER) == AQ::OPTION_LINK_IDENTIFIER;
    for (int i = 0; i < ProducerCount; ++i)
    {
        Producers.push_back(new Producer(consumer, i + 1, sm, 
            PageSizeAlloc, checkLinkId, MaxOutstanding, MaxPagesPerAppend,
            TraceEnableProducer ? Trace : NULL));
#ifdef AQ_TEST_POINT
        if (TpDelayClaimBeforeWriteHeadRef)
        {
            Producers[i]->injectTestPointDelay(AQWriter::ClaimBeforeWriteHeadRef);
        }
        if (TpDelayClaimBeforeWriteCtrlSkipPages)
        {
            Producers[i]->injectTestPointDelay(AQWriter::ClaimBeforeWriteCtrlSkipPages);
        }
        if (TpDelayClaimBeforeWriteCtrl)
        {
            Producers[i]->injectTestPointDelay(AQWriter::ClaimBeforeWriteCtrl);
        }
#endif
    }
    assertShmGuard();

    // Create and start the snapshot takers.
    for (int i = 0; i < SnapshotTakerCount; ++i)
    {
        SnapshotTakers.push_back(new SnapshotTaker(consumer, i + 1 + ProducerCount, MaxSnapshotPeriodMs, Trace));
    }
    assertShmGuard();

    // All producers ready but not running; now we run the produce/consume
    // logic for the number of requested loops.
    size_t runLoop = 0;
    while (RunLoops == 0 || runLoop < RunLoops)
    {
        runLoop++;
        produceConsume(consumer);
        assertShmGuard();
        printStatistics(runLoop);
    }


    // Delete the producers and snapshot takers now that we are finished.
    assertShmGuard();
    for (size_t i = 0; i < SnapshotTakers.size(); ++i)
    {
        delete SnapshotTakers[i];
    }
    SnapshotTakers.clear();
    assertShmGuard();
    for (size_t i = 0; i < Producers.size(); ++i)
    {
        delete Producers[i];
    }
    Producers.clear();
    assertShmGuard();
    delete SnapValidator;

    // Delete the trace manager.
    if (Trace != NULL)
    {
        delete Trace;
        Trace = NULL;
    }

    // Delete the reader items created during this run.
    for (size_t i = 0; i < FreeReaderItems.size(); ++i)
    {
        delete FreeReaderItems[i];
    }
    FreeReaderItems.clear();

    // Delete the shared memory and exit.
    delete [] Shm;
    Shm = NULL;

    return 0;
}

//------------------------------------------------------------------------------
static void produceConsume(AQReader &reader)
{
    Timer::Ms_t prev = 0;
    bool first = true;

    // Start all the producers and snapshot takers for this run.
    for (size_t i = 0; i < Producers.size(); ++i)
    {
        Producers[i]->start();
    }
    for (size_t i = 0; i < SnapshotTakers.size(); ++i)
    {
        SnapshotTakers[i]->start();
    }

    // Run the reader for the consumption time.
    ProduceConsumeState state = ProduceConsumeRunning;
    Timer::Ms_t start = Timer::start();
    AQItem *item = NULL;
    do
    {
        size_t i = CONSUME_LOOP_COUNTER + 1;
        while (state == ProduceConsumeJoined || (--i > 0))
        {
            if (item == NULL)
            {
                item = allocReaderItem();
            }
            Timer::Ms_t now = Timer::start();
            bool res = reader.retrieve(*item);
            if (first)
            {
                first = false;
            }
            else
            {
                Timer::Ms_t elapsed = now - prev;
                if (elapsed > StatMaxConsumePeriodMs)
                {
                    StatMaxConsumePeriodMs = elapsed;
                }
            }
            prev = now;
            if (!res)
            {
                break;
            }
            StatRetreiveSuccess++;
            assertRecord(item);
            item = NULL;
        }
        if (i > 0)
        {
            StatRetreiveFailure++;
        }

        switch (state)
        {
        case ProduceConsumeRunning:
            // Test if we need to stop; if we do need to stop then issue the
            // instruction to all threads to stop.
            if (Timer::elapsed(start) > 1000 * RunTimeSecs)
            {
                state = ProduceConsumeStopping;
                for (size_t i = 0; i < SnapshotTakers.size(); ++i)
                {
                    SnapshotTakers[i]->stop();
                }
                for (size_t i = 0; i < Producers.size(); ++i)
                {
                    Producers[i]->stop();
                }
            }
            break;

        case ProduceConsumeStopping:
        {
            // Check to see if all threads have joined.
            size_t i = 0;
            for (; i < SnapshotTakers.size() && SnapshotTakers[i]->join(0); ++i);
            if (i == SnapshotTakers.size())
            {
                for (i = 0; i < Producers.size() && Producers[i]->join(0); ++i);
                if (i == Producers.size())
                {
                    state = ProduceConsumeJoined;
                }
            }
            if (   state != ProduceConsumeJoined
                && Timer::elapsed(start) > 2000 * RunTimeSecs)
            {
                assertFailed("Lock-up detected; took too long for producers to complete");            
            }
            break;
        }

        case ProduceConsumeJoined:
            for (size_t i = 0; i < Producers.size(); ++i)
            {
                Producers[i]->consumerChannel().assertComplete();
            }
            for (size_t i = 0; i < SnapshotTakers.size(); ++i)
            {
                SnapshotTaker *st = SnapshotTakers[i];

                for (size_t j = 0; j < st->snapshotCount(); ++j)
                {
                    SnapValidator->validate(st->snapshot(j));
                }
                st->clearSnapshots();
            }
            SnapValidator->purge();
            state = ProduceConsumeComplete;
            break;

        default:
            // Take no action.
            break;
        }
    } while (state != ProduceConsumeComplete);
    if (item != NULL)
    {
        freeReaderItem(item);
    }
}

//------------------------------------------------------------------------------
static void printStatistics(int loop)
{
    cout << "Loop " << loop << ":";

    unsigned long long success = StatRetreiveSuccess;
    unsigned long long failure = StatRetreiveFailure;
    unsigned long long total = success + failure;
    int perc = total == 0 ? 0 : (int)((success * 100 + total / 2) / total);

    unsigned long long incomplete = StatRecordIncomplete;
    total = StatRecordComplete + StatRecordIncomplete;
    int cperc = total == 0 ? 0 : (int)((incomplete * 100 + total / 2) / total);

    cout << " con(" << success << " @ " << perc << "%, incomplete " 
        << incomplete << " @ " << cperc << "%, >>" << StatMaxConsumePeriodMs << "ms)";

    for (size_t i = 0; i < Producers.size(); ++i)
    {
        const Producer &p = *Producers[i];

        success = p.getStat(Producer::StatisticClaimSuccess);
        failure = p.getStat(Producer::StatisticClaimFailures);
        total = success + failure;
        perc = total == 0 ? 0 : (int)((success * 100 + total / 2) / total);

        unsigned long long maxCycleMs = p.getStat(Producer::StatisticMaxCycleTimeMs);

        cout << " p" << (i + 1) << "(" << success << " @ " << perc << "%, >>" 
            << maxCycleMs << "ms)";
    }
    for (size_t i = 0; i < SnapshotTakers.size(); ++i)
    {
        const SnapshotTaker &st = *SnapshotTakers[i];

        total = st.getStat(SnapshotTaker::StatisticSnapshotsTaken);

        unsigned long long avgRecs = total == 0 ? 0 : (st.getStat(SnapshotTaker::StatisticTotalRecords) + total / 2) / total;
        unsigned long long maxSnapshotMs = st.getStat(SnapshotTaker::StatisticMaxSnapshotTimeMs);

        cout << " s" << (i + 1) << "(" << total << " @ " << avgRecs << ", >>"
            << maxSnapshotMs << "ms)";
    }
    cout << endl;
}

//------------------------------------------------------------------------------
void assertShmGuard(void)
{
    for (int i = SHM_GUARD_SIZE - 1; i >= 0; --i)
    {
        if (Shm[i] != SHM_GUARD_BYTE)
        {
            ostringstream ss;
            ss << endl << endl << "### SHM_GUARD_UNDERRUN at byte " << i << endl;
            assertFailed(ss.str());
        }
    }
    for (int i = 0; i < SHM_GUARD_SIZE; ++i)
    {
        if (Shm[SHM_GUARD_SIZE + ShmSize + i] != SHM_GUARD_BYTE)
        {
            ostringstream ss;
            ss << endl << endl << "### SHM_GUARD_UNDERRUN at byte " << i << endl;
            assertFailed(ss.str());
        }
    }
}

//------------------------------------------------------------------------------
string parseRecordTag(const char *whom, const AQItem *item,
    unsigned int &recLen, int &threadId, unsigned long long &count)
{
    threadId = -1;
    count = 0;

    // Decode the item.
    if (item->size() < RECORD_MIN_LEN)
    {
        ostringstream ss;
        ss << endl << endl << "### INVALID_RECORD when paring for " << whom 
            << "; size is " << item->size() << " less than minimum of "
            << RECORD_MIN_LEN << endl;
        assertFailed(ss.str());
    }

    char header[RECORD_MIN_LEN + 1];
    memcpy(header, &(*item)[0], RECORD_MIN_LEN);
    header[RECORD_MIN_LEN] = '\0';

    // Check the string for the separator markers.
    if (   header[RECORD_LEN_CHARS] != ','
        || header[RECORD_LEN_CHARS + 1 + RECORD_THREAD_ID_CHARS] != ','
        || header[RECORD_LEN_CHARS + 1 + RECORD_THREAD_ID_CHARS + 1 + RECORD_COUNT_CHARS] != ':')
    {
        ostringstream ss;
        ss << endl << endl << "### INVALID_RECORD when parsing for " << whom 
            << " markers are incorrect in \"" << header << "\"" << endl;
        assertFailed(ss.str());
    }

    // Extract the thread ID and counters.
    sscanf(header, RECORD_FORMAT, &recLen, &threadId, &count);

    // Verify the threadId.
    if (threadId < 1 || threadId >(int)Producers.size())
    {
        ostringstream ss;
        ss << endl << endl << "### INVALID_RECORD when parsing for " << whom 
            << "; threadId " << threadId << " is in incorrect in \"" << header << "\"" << endl;
        assertFailed(ss.str());
    }

    return string(&header[RECORD_LEN_CHARS + 1]);
}

//------------------------------------------------------------------------------
string itemToString(const AQItem *item)
{
    size_t len = 0;
    for (const AQItem *it = item; it != NULL; it = it->next())
    {
        len += it->size();
    }
    char *str = new char[len + 1];
    len = 0;
    for (const AQItem *it = item; it != NULL; it = it->next())
    {
        memcpy(&str[len], &(*it)[0], it->size());
        len += it->size();
    }
    str[len] = '\0';

    string rstr = str;

#ifdef _WIN32
    _snprintf(str, len, "@%p", item);
#else
    snprintf(str, len, "@%p", item);
#endif
    rstr += str;

#ifdef _WIN32
    _snprintf(str, len, " [quid=%08X, lkid=%08X]", item->queueIdentifier(), item->linkIdentifier());
#else
    snprintf(str, len, " [quid=%08X, lkid=%08X]", item->queueIdentifier(), item->linkIdentifier());
#endif
    rstr += str;

    delete[] str;
    return rstr;
}

//------------------------------------------------------------------------------
static void assertRecord(AQItem *item)
{
    // Check the status of the record.
    for (const AQItem *it = item; it != NULL; it = it->next())
    {
        if (it->isCommitted())
        {
            StatRecordComplete++;
        }
        else
        {
            StatRecordIncomplete++;
        }
    }

    if (item->isCommitted() && !item->isChecksumValid())
    {
        assertFailed("### CHECKSUM FAILED");
    }

    unsigned int recLen = 0;
    int threadId = -1;
    unsigned long long count = 0;
    string tag = parseRecordTag("consumer", item, recLen, threadId, count);

    // Store this record in the map used for snapshot validation.
    if (SnapshotTakers.size() > 0)
    {
        SnapValidator->add(tag, item);
    }

    Producers[threadId - 1]->consumerChannel().process(item, recLen, count);
}

//------------------------------------------------------------------------------
void assertFailed(const string& msg)
{
    // Stop all threads from running.
    for (size_t i = 0; i < SnapshotTakers.size(); ++i)
    {
        SnapshotTakers[i]->stopImmediate();
    }
    for (size_t i = 0; i < Producers.size(); ++i)
    {
        Producers[i]->stopImmediate();
    }
    for (size_t i = 0; i < SnapshotTakers.size(); ++i)
    {
        SnapshotTakers[i]->join(10000);
    }
    for (size_t i = 0; i < Producers.size(); ++i)
    {
        Producers[i]->join(10000);
    }

    ofstream out("StressTest_Failure.log");

    logAssertFailed(cerr, msg, NULL);
    logAssertFailed(out, msg, Trace);

    out.close();
    abort();
}

//------------------------------------------------------------------------------
static void logAssertFailed(ostream& out, const string& msg, TraceManager *tm)
{
    out << endl
        << endl 
        << "================================================================================" << endl 
        << msg 
        << endl << "================================================================================" << endl;

    if (tm != NULL)
    {
        tm->write(out);
        out << "================================================================================" << endl 
            << msg 
            << endl << "================================================================================" << endl;
    }
}

//------------------------------------------------------------------------------
static AQItem *allocReaderItem(void)
{
    AQItem *item;

    if (FreeReaderItems.size() == 0)
    {
        item = new AQItem;
    }
    else
    {
        item = FreeReaderItems.back();
        FreeReaderItems.pop_back();
    }
    return item;
}

//------------------------------------------------------------------------------
void freeReaderItem(AQItem *item)
{
    FreeReaderItems.push_back(item);
}

//------------------------------------------------------------------------------
static void configure(Optarg &cfg)
{
    cfg.opt('t', RunTimeSecs, "The duration of each stress loop in seconds.");
    cfg.opt('l', RunLoops, "The number of test loops to perform; a value of 0 loops forever.");
    cfg.opt('p', ProducerCount, "The number of producer threads to create and write into the queue.");
    cfg.opt('s', SnapshotTakerCount, "The number of snapshot taking threads to create and write into the queue.");
    cfg.opt('M', ShmSize, "The size of the shared memory region.");
    cfg.opt('P', PageSizeShift, "The size of each AQ page expressed as 2^(this value).");
    cfg.opt('A', PageSizeAlloc, "Each allocation consists of a number of pages; this is a colon separated list of page counts that are valid allocation sizes.");
    cfg.opt('T', CommitTimeoutMs, "The commit timeout in milliseconds.");
    cfg.opt('C', AQ::OPTION_CRC32, FormatOptions, "Enables the CRC-32 queue formatting option.");
    cfg.opt('L', AQ::OPTION_LINK_IDENTIFIER, FormatOptions, "Enables the link identifier queue formatting option.");
    cfg.opt('E', AQ::OPTION_EXTENDABLE, FormatOptions, "Enables the extendable queue formatting option.");
    cfg.opt('O', MaxOutstanding, "The maximum number of items that a produce may hold outstanding without committing them.");
    cfg.opt('W', MaxPagesPerAppend, "The maximum number of pages to write in any one append operation when running with the extendable option.");
    cfg.opt('S', MaxSnapshotPeriodMs, "The maximum amount of time (in milliseconds) between snapshot capture.");

#ifdef AQ_TEST_POINT
    cfg.opt('1', TpDelayClaimBeforeWriteHeadRef, "If set then a 1ms sleep is introduced immediatly before each head reference is written in AQWriter::commit().");
    cfg.opt('2', TpDelayClaimBeforeWriteCtrlSkipPages, "If set then a 1ms sleep is introduced immediatly before the control register for wasted pages is written in AQWriter::commit().");
    cfg.opt('3', TpDelayClaimBeforeWriteCtrl, "If set then a 1ms sleep is introduced immediatly before the control register is written in AQWriter::commit().");
#endif

    if (cfg.hasOpt('h', "Show the command line option help."))
    {
        cout << endl << cfg.helpMessage() << endl;
        exit(0);
    }
}




//=============================== End of File ==================================
