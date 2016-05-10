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




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

// Do this in order to cause all the items to show into the same file.
#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"

//------------------------------------------------------------------------------
TEST_SUITE(UtAQLogEncodeDecode);

/*
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor 
incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis 
nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. 
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore 
eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt
in culpa qui officia deserunt mollit anim id est laborum.
*/
//------------------------------------------------------------------------------
TEST(given_LogCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 907 "sit amet.c"
    AQLog_Critical("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TCritical(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 907 "sit amet.c"
    AQLog_DCritical(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDCritical_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_CRITICAL);
#line 907 "sit amet.c"
    AQLog_TDCritical(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_CRITICAL,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 907 "sit amet.c"
    AQLog_Error("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_ERROR,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TError(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_ERROR,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 907 "sit amet.c"
    AQLog_DError(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_ERROR,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDError_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_ERROR);
#line 907 "sit amet.c"
    AQLog_TDError(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_ERROR,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 907 "sit amet.c"
    AQLog_Warning("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_WARNING,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TWarning(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_WARNING,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 907 "sit amet.c"
    AQLog_DWarning(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_WARNING,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDWarning_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_WARNING);
#line 907 "sit amet.c"
    AQLog_TDWarning(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_WARNING,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 907 "sit amet.c"
    AQLog_Notice("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TNotice(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 907 "sit amet.c"
    AQLog_DNotice(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDNotice_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_NOTICE);
#line 907 "sit amet.c"
    AQLog_TDNotice(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_NOTICE,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 907 "sit amet.c"
    AQLog_Info("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_INFO,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TInfo(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_INFO,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 907 "sit amet.c"
    AQLog_DInfo(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_INFO,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDInfo_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_INFO);
#line 907 "sit amet.c"
    AQLog_TDInfo(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_INFO,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 907 "sit amet.c"
    AQLog_Detail("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TDetail(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 907 "sit amet.c"
    AQLog_DDetail(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDDetail_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DETAIL);
#line 907 "sit amet.c"
    AQLog_TDDetail(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_DETAIL,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 907 "sit amet.c"
    AQLog_Debug("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TDebug(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 907 "sit amet.c"
    AQLog_DDebug(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDDebug_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_DEBUG);
#line 907 "sit amet.c"
    AQLog_TDDebug(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_DEBUG,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 907 "sit amet.c"
    AQLog_Trace("%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_TRACE,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "consectetur"
    ProcessIdentifier::fixProcessId("adipiscing", 0, 0);
    Timestamp::fixTimestamp(1ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 7366533 "elit/sed\\do/eiusmod.c"
    AQLog_TTrace(tempor, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireMessage(rec, LONG_STR_445 ":" LONG_STR_844, "consectetur", "tempor", "eiusmod.c", 7366533, __FUNCTION__, AQLOG_LEVEL_TRACE,
        1ULL, "adipiscing", 0, 0);
}

//------------------------------------------------------------------------------
TEST(given_LogDTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 907 "sit amet.c"
    AQLog_DTrace(BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_TRACE,
        939920390332ULL, "dolor.exe", 87332, 340002);
}

//------------------------------------------------------------------------------
TEST(given_LogTDTrace_when_RecordRetrieved_then_FieldsMatch)
{
    // Preconditions.
#undef AQLOG_COMPONENT_ID
#define AQLOG_COMPONENT_ID "Lorem ipsum"
    ProcessIdentifier::fixProcessId("dolor.exe", 87332, 340002);
    Timestamp::fixTimestamp(939920390332ULL);
    LogReaderTest log(AQLOG_LEVEL_TRACE);
#line 907 "sit amet.c"
    AQLog_TDTrace(incididunt, BinaryData_g, sizeof(BinaryData_g) - 1, "%s:%s", LONG_STR_445, LONG_STR_844);

#line 1 "UtAQLogEncodeDecode_FILE_LINE_NOT_SUPPORTED.cpp"
    AQLogRecord *rec = log.nextRecordAfterEmpty();
    log.requireData(rec, LONG_STR_445 ":" LONG_STR_844, BinaryData_g, sizeof(BinaryData_g) - 1, "Lorem ipsum", "incididunt", "sit amet.c", 907, __FUNCTION__, AQLOG_LEVEL_TRACE,
        939920390332ULL, "dolor.exe", 87332, 340002);
}




//=============================== End of File ==================================
