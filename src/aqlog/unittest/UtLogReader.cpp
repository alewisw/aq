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

#include "LogReaderTest.h"

#include "Timestamp.h"
#include "Timer.h"

using namespace aq;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

#define AQLOG_COMPONENT_ID              "aqlog_unittest"




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
TEST_SUITE_FIRST(UtLogReader);

//------------------------------------------------------------------------------
#ifdef AQ_TEST_UNIT
TEST(given_RecordTimeT3_when_RecordTimeT1AndT2Submitted_then_RecordsReadInTimestampOrder)
{
    uint32_t ms = 0;
    LogReaderTest log(AQLOG_LEVEL_INFO);
    TestHandler h;

    Timestamp::fixTimestamp(1234234765);
    AQLog_Info("T3");
    REQUIRE(log.reader.retrieve(ms) == NULL);
    REQUIRE(ms == LogReader::PENDING_MINIMUM_WINDOW_MS);

    Timestamp::fixTimestamp(1234234763);
    AQLog_Info("T1");
    Timer::sleep(LogReader::PENDING_MINIMUM_WINDOW_MS / 2);
    REQUIRE(log.reader.retrieve(ms) == NULL);
    REQUIRE(ms == LogReader::PENDING_MINIMUM_WINDOW_MS);

    Timestamp::fixTimestamp(1234234764);
    AQLog_Info("T2");
    Timer::sleep(LogReader::PENDING_MINIMUM_WINDOW_MS / 2);
    REQUIRE(log.reader.retrieve(ms) == NULL);
    REQUIRE(ms == LogReader::PENDING_MINIMUM_WINDOW_MS / 2);

    Timer::sleep(LogReader::PENDING_MINIMUM_WINDOW_MS + 1);
    AQLogRecord *rec;
    REQUIRE((rec = log.reader.retrieve(ms)) != NULL);
    REQUIRE(rec->message().toString() == "T1");
    REQUIRE((rec = log.reader.retrieve(ms)) != NULL);
    REQUIRE(rec->message().toString() == "T2");
    REQUIRE((rec = log.reader.retrieve(ms)) != NULL);
    REQUIRE(rec->message().toString() == "T3");
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_UNIT
TEST(given_RecordTimeTFarInFuture_when_RecordsAtCurrentTimeConstantlySubmitted_then_FutureRecordReturnedAfterMaxHoldTime)
{
    uint32_t ms = 0;
    LogReaderTest log(AQLOG_LEVEL_INFO);
    TestHandler h;

    Timestamp::fixTimestamp(2000000000);
    AQLog_Info("Tfuture");
    REQUIRE(log.reader.retrieve(ms) == NULL);
    REQUIRE(ms == LogReader::PENDING_MINIMUM_WINDOW_MS);

    for (size_t i = 0; i < 200; ++i)
    {
        Timestamp::fixTimestamp(i);
        AQLog_Info("T%d", i);
        Timer::sleep((LogReader::PENDING_MAXIMUM_WINDOW_MS + 99) / 100);
        AQLogRecord *rec = log.reader.retrieve(ms);
        if (rec != NULL && rec->message().toString() == "Tfuture")
        {
            return;
        }
    }
    REQUIRE(false);
}
#endif

//------------------------------------------------------------------------------
TEST(given_RecordRetrieved_when_ReleaseRecordWithBadAddress_then_InvalidArgumentException)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);

    AQLog_Info("T");
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    REQUIRE(rec != NULL);
    REQUIRE_EXCEPTION(log.reader.release(&rec[1]), invalid_argument);
}

//------------------------------------------------------------------------------
TEST(given_RecordRetrieved_when_ReleaseNullRecord_then_InvalidArgumentException)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);

    AQLog_Info("T");
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    REQUIRE(rec != NULL);
    REQUIRE_EXCEPTION(log.reader.release(NULL), invalid_argument);
}

//------------------------------------------------------------------------------
TEST(given_RecordCorrupted_when_LogReaderRetrieve_then_RecordNotRetrieved)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    static const char matchStr[] = "tag_corrupt_flag";

    AQLog_TInfo(tag_corrupt_flag, "T");
    IAQSharedMemory& aqm = log.aqMemory();
    char *ptr = (char *)aqm.baseAddress();
    bool corrupted = false;
    for (size_t i = 0; i < aqm.size() - sizeof(matchStr); ++i)
    {
        if (memcmp(&ptr[i], matchStr, sizeof(matchStr)) == 0)
        {
            ptr[i + sizeof(matchStr) - 1] = 'x';
            corrupted = true;
            break;
        }
    }
    CHECK(corrupted);
    for (size_t i = 0; i < 3; ++i)
    {
        uint32_t ms;
        AQLogRecord *rec = log.reader.retrieve(ms);
        REQUIRE(rec == NULL);
        Timer::sleep(ms);
    }
}



//=============================== End of File ==================================
