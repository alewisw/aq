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
#include "AQSnapshot.h"
#include "AQWriter.h"
#include "AQWriterItem.h"

#include "Timer.h"

using namespace aqlog;
using namespace aqosa;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// Markers used in the test log record.
#define MARK_COMPONENT_ID               "componentId"
#define MARK_TAG_ID                     "tagId"
#define MARK_FILE                       "file"
#define MARK_FUNC                       "function"
#define MARK_MESSAGE                    "message"




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------

typedef enum ACTION_EN
{
    // No special action.
    ACTION_NONE,

    // Corrupt a checksum.
    ACTION_CORRUPT_CRC,

    // Create an incomplete record.
    ACTION_NO_COMMIT,

} Action_en;



//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Returns the test record by passing it through the log.
static vector<char> getTestRecord(LogReaderTest &log);

// Converts raw log string data into an AQItem which is then set in the passed
// test record.
static void setTestRecord(AQLogRecord& rec, const vector<char>& data, 
    Action_en act = ACTION_NONE);

// Gets a test record, removes the nul terminator from the specified string (and
// all other characters after it) and returns the population result code.
static AQLogRecord::PopulateOutcome truncateStringAndPopulate(AQLogRecord& rec,
    const string& str);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtAQLogRecord);

//------------------------------------------------------------------------------
TEST(given_LogRecordNormal_when_Populate_Success)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    vector<char> data = getTestRecord(log);

    AQLogRecord rec;
    setTestRecord(rec, data);
    REQUIRE(rec.populate() == AQLogRecord::POPULATE_SUCCESS);
    REQUIRE(string(rec.componentId()) == MARK_COMPONENT_ID);
    REQUIRE(string(rec.tagId()) == MARK_TAG_ID);
    REQUIRE(string(rec.file()) == MARK_FILE);
    REQUIRE(string(rec.function()) == MARK_FUNC);
    REQUIRE(string(rec.processName()) == ProcessIdentifier::currentProcessName());
    REQUIRE(rec.message().toString() == MARK_MESSAGE);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedHeader_when_Populate_ErrorTruncatedHeader)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    vector<char> data = getTestRecord(log);

    data.erase(data.begin() + offsetof(AQLogRecord::Overlay, strData) - 1, data.end());

    AQLogRecord rec;
    setTestRecord(rec, data);
    REQUIRE(rec.populate() == AQLogRecord::POPULATE_ERROR_TRUNCATED_HEADER);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedData_when_Populate_ErrorTruncatedData)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    vector<char> data = getTestRecord(log);

    data.erase(data.begin() + offsetof(AQLogRecord::Overlay, strData) + sizeof(BinaryData_g) - 1, data.end());

    AQLogRecord rec;
    setTestRecord(rec, data);
    REQUIRE(rec.populate() == AQLogRecord::POPULATE_ERROR_TRUNCATED_DATA);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordCorrupted_when_Populate_ErrorChecksum)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    vector<char> data = getTestRecord(log);

    AQLogRecord rec;
    setTestRecord(rec, data, ACTION_CORRUPT_CRC);
    REQUIRE(rec.populate() == AQLogRecord::POPULATE_ERROR_CHECKSUM);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordUncommitted_when_Populate_ErrorUncommitted)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    vector<char> data = getTestRecord(log);

    AQLogRecord rec;
    setTestRecord(rec, data, ACTION_NO_COMMIT);
    REQUIRE(rec.populate() == AQLogRecord::POPULATE_ERROR_UNCOMMITTED);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedComponentId_when_Populate_ErrorTruncatedComponentId)
{
    AQLogRecord rec;
    REQUIRE(truncateStringAndPopulate(rec, MARK_COMPONENT_ID) == AQLogRecord::POPULATE_ERROR_TRUNCATED_COMPONENT_ID);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedTagId_when_Populate_ErrorTruncatedTagId)
{
    AQLogRecord rec;
    REQUIRE(truncateStringAndPopulate(rec, MARK_TAG_ID) == AQLogRecord::POPULATE_ERROR_TRUNCATED_TAG_ID);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedFile_when_Populate_ErrorTruncatedFile)
{
    AQLogRecord rec;
    REQUIRE(truncateStringAndPopulate(rec, MARK_FILE) == AQLogRecord::POPULATE_ERROR_TRUNCATED_FILE);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedFunction_when_Populate_ErrorTruncatedFunction)
{
    AQLogRecord rec;
    REQUIRE(truncateStringAndPopulate(rec, MARK_FUNC) == AQLogRecord::POPULATE_ERROR_TRUNCATED_FUNCTION);
}

//------------------------------------------------------------------------------
TEST(given_LogRecordTruncatedProcessName_when_Populate_ErrorTruncatedProcessName)
{
    AQLogRecord rec;
    REQUIRE(truncateStringAndPopulate(rec, ProcessIdentifier::currentProcessName())
        == AQLogRecord::POPULATE_ERROR_TRUNCATED_PROCESS_NAME);
}

//------------------------------------------------------------------------------
static AQLogRecord::PopulateOutcome truncateStringAndPopulate(AQLogRecord& rec, 
    const string& str)
{
    LogReaderTest log(AQLOG_LEVEL_INFO);
    vector<char> data = getTestRecord(log);

    for (size_t i = 0; i < data.size() - str.size(); ++i)
    {
        if (memcmp(&data[i], str.c_str(), str.size()) == 0)
        {
            data.erase(data.begin() + i + str.size(), data.end());
            break;
        }
    }

    setTestRecord(rec, data);
    return rec.populate();
}

//------------------------------------------------------------------------------
static vector<char> getTestRecord(LogReaderTest &log)
{
    __AQLog_Write(AQLOG_LEVEL_NOTICE,
        MARK_COMPONENT_ID, sizeof(MARK_COMPONENT_ID),
        MARK_TAG_ID, sizeof(MARK_TAG_ID),
        MARK_FILE, sizeof(MARK_FILE),
        MARK_FUNC, sizeof(MARK_FUNC),
        __LINE__,
        BinaryData_g, sizeof(BinaryData_g),
        "%s", MARK_MESSAGE);
    AQLogRecord *rec = log.nextLevelRecord(AQLOG_LEVEL_NOTICE);
    CHECK(rec != NULL);
    CHECK(rec->aqItem().next() == NULL);
    const unsigned char *raw = &rec->aqItem()[0];
    vector<char> v;
    for (size_t i = 0; i < rec->aqItem().size(); ++i)
    {
        v.push_back((char)raw[i]);
    }
    return v;
}

//------------------------------------------------------------------------------
static void setTestRecord(AQLogRecord& rec, const vector<char>& data, 
    Action_en act)
{
    static uint32_t mem[10000];
    AQExternMemory sm(mem, sizeof(mem));
    AQReader reader(sm);
    CHECK(reader.format(8, 1000, AQ::OPTION_EXTENDABLE | (act == ACTION_CORRUPT_CRC ? AQ::OPTION_CRC32 : 0)));
    AQWriter writer(sm);

    AQWriterItem witem;
    CHECK(writer.claim(witem, data.size()));
    CHECK(witem.write(&data[0], data.size()));
    unsigned char *ptr = &witem[0];

    if (act == ACTION_NO_COMMIT)
    {
        rec.aqItem() = witem;
    }
    else
    {
        CHECK(writer.commit(witem));

        if (act == ACTION_CORRUPT_CRC)
        {
            *ptr = *ptr ^ 0x5A;
        }

        CHECK(reader.retrieve(rec.aqItem()));
    }
}




//=============================== End of File ==================================
