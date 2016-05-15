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
#include "DataSets.h"
#include "LogReaderTest.h"

#include "AQLog.h"

#include "LogLevelHash.h"
#include "LogMemory.h"
#include "LogReader.h"

#include "ProcessIdentifier.h"
#include "Timestamp.h"

#include "AQHeapMemory.h"
#include "AQExternMemory.h"
#include "AQWriter.h"

#include "Timer.h"

using namespace aqlog;
using namespace aqosa;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "aqlog"





//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Returns the size of a file as it is saved to the log.
static size_t savedSizeOfFile(const char *file);



//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtAQLog);

//------------------------------------------------------------------------------
static size_t savedSizeOfFile(const char *file)
{
    const char *p = file;
    while (*p != '\0')
    {
        if (*p == '/' || *p == '\\')
        {
            file = &p[1];
        }
        p++;
    }
    return strlen(p);
}

//------------------------------------------------------------------------------
TEST(given_MemoryRegionTooSmall_when_AQLogInit_then_ErrorReturned)
{
    AQHeapMemory mem(AQLOG_SHM_MINIMUM_SIZE - 1);
    REQUIRE(AQLog_InitSharedMemory(mem) == AQLOG_INITOUTCOME_SHM_TOO_SMALL);
}

//------------------------------------------------------------------------------
TEST(given_AQUnformatted_when_AQLogInit_then_ErrorReturned)
{
    AQHeapMemory mem(AQLOG_SHM_MINIMUM_SIZE);
    memset(mem.baseAddress(), 0xCD, mem.size());
    REQUIRE(AQLog_InitSharedMemory(mem) == AQLOG_INITOUTCOME_AQ_UNFORMATTED);
}

//------------------------------------------------------------------------------
TEST(given_LogQueueCanOnlyContainTruncatedData_when_WriteLargeData_then_DataTruncated)
{
    size_t maxSpace = sizeof(AQLogRecord::Overlay) 
        + sizeof(BinaryData_g) 
        + ProcessIdentifier::currentProcessName().size() + 1
        + sizeof(AQLOG_COMPONENT_ID) 
        + sizeof("") 
        + savedSizeOfFile(__FILE__) 
        + sizeof(__FUNCTION__) 
        + AQLOG_RESERVE_MESSAGE_SIZE
        - 1;

    LogReaderTest log(AQLOG_LEVEL_INFO);
    while (log.aq.availableSize() > maxSpace)
    {
        AQLog_Info("");
    }

    AQLog_DNotice(BinaryData_g, sizeof(BinaryData_g), "%s%s", "Lorem ipsum dolor sit", " amet");

    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    log.requireTruncatedData(rec, "Lorem ipsum dolor sit amet", BinaryData_g,
        AQLOG_COMPONENT_ID, "", __FILE__, 0, __FUNCTION__, AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());

    AQLog_Notice("");
    rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());
}

//------------------------------------------------------------------------------
TEST(given_LogQueueCannotContainTruncatedData_when_WriteLargeData_then_NothingWrittenAndOutOfSpaceFlagSet)
{
    size_t maxSpace = sizeof(AQLogRecord::Overlay)
        + AQLOG_DATA_TRUNCATE_SIZE - 1
        + ProcessIdentifier::currentProcessName().size() + 1
        + sizeof(AQLOG_COMPONENT_ID)
        + sizeof("")
        + savedSizeOfFile(__FILE__)
        + sizeof(__FUNCTION__)
        + AQLOG_RESERVE_MESSAGE_SIZE
        - 1;

    LogReaderTest log(AQLOG_LEVEL_INFO);
    while (log.aq.availableSize() > maxSpace)
    {
        AQLog_Info("");
    }

    AQLog_DNotice(BinaryData_g, sizeof(BinaryData_g), "%s%s", "Lorem ipsum dolor sit", " amet");

    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(rec == NULL);

    AQLog_Notice("");
    rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(rec->isReportingOutOfSpaceDrop());
}

//------------------------------------------------------------------------------
TEST(given_LogQueueCannotContainMessage_when_WriteEmptyMessage_then_NothingWrittenAndOutOfSpaceFlagSet)
{
    size_t maxSpace = sizeof(AQLogRecord::Overlay)
        + 0
        + ProcessIdentifier::currentProcessName().size() + 1
        + sizeof(AQLOG_COMPONENT_ID)
        + sizeof("")
        + savedSizeOfFile(__FILE__)
        + sizeof(__FUNCTION__)
        + AQLOG_RESERVE_MESSAGE_SIZE
        - 1;

    LogReaderTest log(AQLOG_LEVEL_INFO);
    while (log.aq.availableSize() > maxSpace)
    {
        AQLog_Info("");
    }

    AQLog_Notice("");
    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(rec == NULL);

    AQLog_Notice("");
    rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(rec->isReportingOutOfSpaceDrop());
}

//------------------------------------------------------------------------------
TEST(given_LogQueueCannotContainWholeMessage_when_WriteLargeMessage_then_MessageTruncated)
{
    size_t maxSpace = sizeof(AQLogRecord::Overlay)
        + ProcessIdentifier::currentProcessName().size() + 1
        + sizeof(AQLOG_COMPONENT_ID)
        + sizeof("")
        + savedSizeOfFile(__FILE__)
        + sizeof(__FUNCTION__)
        + sizeof(LONG_STR_844) - 1
        - 1;

    LogReaderTest log(AQLOG_LEVEL_INFO);
    while (log.aq.availableSize() > maxSpace)
    {
        AQLog_Info("");
    }

    AQLog_Notice("%s", LONG_STR_844);
    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    log.requireTruncatedMessage(rec, LONG_STR_844, 
        AQLOG_COMPONENT_ID, "", __FILE__, 0, __FUNCTION__, AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());

    AQLog_Notice("");
    rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());
}

//------------------------------------------------------------------------------
TEST(given_LogQueueCannotContainWholeMessage_when_WriteLargeDataLargeMessage_then_DataAndMessageTruncated)
{
    size_t maxSpace = sizeof(AQLogRecord::Overlay)
        + ProcessIdentifier::currentProcessName().size() + 1
        + sizeof(AQLOG_COMPONENT_ID)
        + sizeof("")
        + savedSizeOfFile(__FILE__)
        + sizeof(__FUNCTION__)
        + sizeof(LONG_STR_865) - 1
        - 1;

    LogReaderTest log(AQLOG_LEVEL_INFO);
    while (log.aq.availableSize() > maxSpace)
    {
        AQLog_Info("");
    }

    AQLog_DNotice(BinaryData_g, sizeof(BinaryData_g), "%s", LONG_STR_865);
    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    log.requireTruncatedMessageData(rec, LONG_STR_865, BinaryData_g,
        AQLOG_COMPONENT_ID, "", __FILE__, 0, __FUNCTION__, AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());

    AQLog_Notice("");
    rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());
}




//=============================== End of File ==================================
