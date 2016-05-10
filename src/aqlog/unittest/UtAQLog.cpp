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

#include "AQExternMemory.h"
#include "AQWriter.h"

#include "Timer.h"

using namespace aqlog;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "aqlog"

#define LONG_STR_445            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
#define LONG_STR_865            "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?"
#define LONG_STR_844            "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat."





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
TEST_SUITE(UtAQLog);

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
