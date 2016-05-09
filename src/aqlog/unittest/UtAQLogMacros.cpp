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
static const uint32_t DataTable[] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706AF48F, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xE0D5E91E, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};



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
TEST_SUITE_FIRST(UtAQLogWriter);

//------------------------------------------------------------------------------
TEST(given_LogQueueCanOnlyContainTruncatedData_when_WriteLargeData_then_DataTruncated)
{
    size_t maxSpace = sizeof(AQLogRecord::Overlay) 
        + sizeof(DataTable) 
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

    AQLog_DNotice(DataTable, sizeof(DataTable), "%s%s", "Lorem ipsum dolor sit", " amet");

    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    log.requireTruncatedData(rec, "Lorem ipsum dolor sit amet", DataTable,
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

    AQLog_DNotice(DataTable, sizeof(DataTable), "%s%s", "Lorem ipsum dolor sit", " amet");

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

    AQLog_DNotice(DataTable, sizeof(DataTable), "%s", LONG_STR_865);
    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    log.requireTruncatedMessageData(rec, LONG_STR_865, DataTable,
        AQLOG_COMPONENT_ID, "", __FILE__, 0, __FUNCTION__, AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());

    AQLog_Notice("");
    rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    REQUIRE(!rec->isReportingOutOfSpaceDrop());
}

// Do this in order to cause all the items to show into the same file.
#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"

//------------------------------------------------------------------------------
TEST_SUITE(UtAQLogMacros);

//------------------------------------------------------------------------------
TEST(given_LogCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 907 "magic.c"
    AQLog_Critical("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 7366533 "super/duper.c"
    AQLog_TCritical(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 907 "magic.c"
    AQLog_DCritical(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 907 "magic.c"
    AQLog_TDCritical(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 907 "magic.c"
    AQLog_Error("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_ERROR,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 7366533 "super/duper.c"
    AQLog_TError(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_ERROR,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 907 "magic.c"
    AQLog_DError(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_ERROR,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 907 "magic.c"
    AQLog_TDError(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_ERROR,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 907 "magic.c"
    AQLog_Warning("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_WARNING,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 7366533 "super/duper.c"
    AQLog_TWarning(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_WARNING,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 907 "magic.c"
    AQLog_DWarning(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_WARNING,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 907 "magic.c"
    AQLog_TDWarning(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_WARNING,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 907 "magic.c"
    AQLog_Notice("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 7366533 "super/duper.c"
    AQLog_TNotice(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 907 "magic.c"
    AQLog_DNotice(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 907 "magic.c"
    AQLog_TDNotice(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 907 "magic.c"
    AQLog_Info("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_INFO,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 7366533 "super/duper.c"
    AQLog_TInfo(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_INFO,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 907 "magic.c"
    AQLog_DInfo(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_INFO,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 907 "magic.c"
    AQLog_TDInfo(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_INFO,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 907 "magic.c"
    AQLog_Detail("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 7366533 "super/duper.c"
    AQLog_TDetail(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 907 "magic.c"
    AQLog_DDetail(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 907 "magic.c"
    AQLog_TDDetail(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 907 "magic.c"
    AQLog_Debug("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 7366533 "super/duper.c"
    AQLog_TDebug(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 907 "magic.c"
    AQLog_DDebug(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 907 "magic.c"
    AQLog_TDDebug(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 907 "magic.c"
    AQLog_Trace("foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_TRACE,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foozler"
    ProcessIdentifier::fixProcessId("finnigan99", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 7366533 "super/duper.c"
    AQLog_TTrace(oogle, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, "foo bar", "foozler", "oogle", "duper.c", 7366533, __FUNCTION__, AQLOG_LEVEL_TRACE,
        1ULL, "finnigan99", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 907 "magic.c"
    AQLog_DTrace(DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_TRACE,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "foo"
    ProcessIdentifier::fixProcessId("fnagle.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 907 "magic.c"
    AQLog_TDTrace(implyTheBest, DataTable, sizeof(DataTable) - 1, "foo %s", "bar");

#line 1 "UtAQLogMacros_FILE_LINE_NOT_SUPPORTED_AFTER_THIS_LINE.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, "foo bar", DataTable, sizeof(DataTable) - 1, "foo", "implyTheBest", "magic.c", 907, __FUNCTION__, AQLOG_LEVEL_TRACE,
        939920390332ULL, "fnagle.exe", 87332, 340002);
}




//=============================== End of File ==================================
