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



//=============================== End of File ==================================
