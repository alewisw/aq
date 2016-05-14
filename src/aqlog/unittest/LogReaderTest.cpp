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

#include "ProcessIdentifier.h"
#include "Timestamp.h"

#include "AQLogRecord.h"

#include "Timer.h"

using namespace std;



//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




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
LogReaderTest::LogReaderTest(AQLogLevel_t level, string tier1, string tier2, 
    string tier3)
    : m_sm(clearMemory(m_mem, sizeof(m_mem)), sizeof(m_mem))
    , m_logMem(m_sm)
    , m_handler(level, tier1, tier2, tier3)
    , reader(m_logMem.aqMemory())
    , hash(m_logMem.logLevelHashMemory())
    , aq(m_logMem.aqMemory())
{
    AQLog_InitSharedMemory(m_sm);

    hash.addHandler(&m_handler);
}

//------------------------------------------------------------------------------
LogReaderTest::~LogReaderTest(void)
{
    AQLog_Deinit();
#ifdef AQ_TEST_UNIT
    Timestamp::fixTimestamp(0);
    ProcessIdentifier::fixProcessId(NULL, 0, 0);
#endif
}

//------------------------------------------------------------------------------
AQLogRecord *LogReaderTest::nextRecordAfterEmpty(void)
{
    uint32_t maxRecallMs;
    AQLogRecord *rec = reader.retrieve(maxRecallMs);
    CHECK(rec == NULL);
    aq::Timer::sleep(maxRecallMs + 25);
    rec = reader.retrieve(maxRecallMs);
    REQUIRE(rec != NULL);
    return rec;
}

//------------------------------------------------------------------------------
AQLogRecord *LogReaderTest::nextLevelRecord(AQLogLevel_t level)
{
    int sleepCount = 0;
    for (;;)
    {
        uint32_t maxRecallMs;
        AQLogRecord *rec = reader.retrieve(maxRecallMs);
        if (rec == NULL)
        {
            if (sleepCount == 3)
            {
                return NULL;
            }
            aq::Timer::sleep(maxRecallMs + 25);
            sleepCount++;
        }
        else
        {
            sleepCount = 0;
            if (rec->level() == level)
            {
                return rec;
            }
            else
            {
                reader.release(rec);
            }
        }
    }
}

//------------------------------------------------------------------------------
void LogReaderTest::requireHeader(const AQLogRecord *rec, const char *componentId,
    const char *tagId, const char *file, uint32_t line, const char *function,
    AQLogLevel_t level, uint64_t timestampNs, const char *processName,
    uint32_t processId, uint32_t threadId)
{
    REQUIRE(rec != NULL);
    REQUIRE(string(componentId) == rec->componentId());
    REQUIRE(string(tagId) == rec->tagId());

    const char *p = file;
    while (*p != '\0')
    {
        if (*p == '/' || *p == '\\')
        {
            file = &p[1];
        }
        p++;
    }

    REQUIRE(string(file) == rec->file());
    if (line > 0)
    {
        REQUIRE(line == rec->lineNumber());
    }
    REQUIRE(string(function) == rec->function());
    REQUIRE(level == rec->level());

    // Check the originating process.
#ifdef AQ_TEST_UNIT
    if (processName != NULL)
    {
        REQUIRE(string(processName) == rec->processName());
        REQUIRE(processId == rec->processId());
        REQUIRE(threadId == rec->threadId());
    }
    else
#endif
    {
        REQUIRE(ProcessIdentifier::currentProcessName() == rec->processName());
        REQUIRE(ProcessIdentifier::currentProcessId() == rec->processId());
        REQUIRE(ProcessIdentifier::currentThreadId() == rec->threadId());
    }

    // Check the timestamp.
#ifdef AQ_TEST_UNIT
    if (timestampNs > 0)
    {
        REQUIRE(timestampNs == rec->timestampNs());
    }
    else
#endif
    {
        REQUIRE(Timestamp::now() >= rec->timestampNs());
    }
}

//------------------------------------------------------------------------------
void LogReaderTest::requireMessage(const AQLogRecord *rec, const char *message,
    const char *componentId, const char *tagId, const char *file,
    uint32_t line, const char *function, AQLogLevel_t level,
    uint64_t timestampNs, const char *processName,
    uint32_t processId, uint32_t threadId)
{
    requireHeader(rec, componentId, tagId, file, line, function, level,
        timestampNs, processName, processId, threadId);

    REQUIRE(!rec->isMessageTruncated());
    REQUIRE(string(message) == rec->message().toString());
    REQUIRE(!rec->isDataTruncated());
    REQUIRE(rec->dataSize() == 0);
}

//------------------------------------------------------------------------------
void LogReaderTest::requireTruncatedMessage(const AQLogRecord *rec, const char *message,
    const char *componentId, const char *tagId, const char *file,
    uint32_t line, const char *function, AQLogLevel_t level,
    uint64_t timestampNs, const char *processName,
    uint32_t processId, uint32_t threadId)
{
    requireHeader(rec, componentId, tagId, file, line, function, level,
        timestampNs, processName, processId, threadId);

    REQUIRE(rec->isMessageTruncated());
    string cmpMessage(message);
    string recMessage = rec->message().toString();
    REQUIRE(recMessage.size() < cmpMessage.size());
    REQUIRE(cmpMessage.substr(0, recMessage.size()) == recMessage);
    REQUIRE(!rec->isDataTruncated());
    REQUIRE(rec->dataSize() == 0);
}

//------------------------------------------------------------------------------
void LogReaderTest::requireData(const AQLogRecord *rec, const char *message,
    const void *data, size_t dataSize, const char *componentId,
    const char *tagId, const char *file, uint32_t line,
    const char *function, AQLogLevel_t level, uint64_t timestampNs,
    const char *processName, uint32_t processId,
    uint32_t threadId)
{
    requireHeader(rec, componentId, tagId, file, line, function, level,
        timestampNs, processName, processId, threadId);

    REQUIRE(!rec->isMessageTruncated());
    REQUIRE(string(message) == rec->message().toString());
    REQUIRE(!rec->isDataTruncated());
    REQUIRE(dataSize == rec->dataSize());
    REQUIRE(memcmp(rec->data(), data, dataSize) == 0);
}

//------------------------------------------------------------------------------
void LogReaderTest::requireTruncatedData(const AQLogRecord *rec, const char *message,
    const void *data, const char *componentId, const char *tagId,
    const char *file, uint32_t line, const char *function,
    AQLogLevel_t level, uint64_t timestampNs,
    const char *processName, uint32_t processId,
    uint32_t threadId)
{
    requireHeader(rec, componentId, tagId, file, line, function, level,
        timestampNs, processName, processId, threadId);

    REQUIRE(!rec->isMessageTruncated());
    REQUIRE(string(message) == rec->message().toString());
    REQUIRE(rec->isDataTruncated());
    REQUIRE(AQLOG_DATA_TRUNCATE_SIZE == rec->dataSize());
    REQUIRE(memcmp(rec->data(), data, AQLOG_DATA_TRUNCATE_SIZE) == 0);
}

//------------------------------------------------------------------------------
void LogReaderTest::requireTruncatedMessageData(const AQLogRecord *rec, const char *message,
    const void *data, const char *componentId, const char *tagId,
    const char *file, uint32_t line, const char *function,
    AQLogLevel_t level, uint64_t timestampNs,
    const char *processName, uint32_t processId,
    uint32_t threadId)
{
    requireHeader(rec, componentId, tagId, file, line, function, level,
        timestampNs, processName, processId, threadId);

    REQUIRE(rec->isMessageTruncated());
    string cmpMessage(message);
    string recMessage = rec->message().toString();
    REQUIRE(recMessage.size() < cmpMessage.size());
    REQUIRE(cmpMessage.substr(0, recMessage.size()) == recMessage);
    REQUIRE(rec->isDataTruncated());
    REQUIRE(AQLOG_DATA_TRUNCATE_SIZE == rec->dataSize());
    REQUIRE(memcmp(rec->data(), data, AQLOG_DATA_TRUNCATE_SIZE) == 0);
}

//------------------------------------------------------------------------------
void *LogReaderTest::clearMemory(void *mem, size_t memSize)
{
    memset(mem, 0, memSize);
    return mem;
}




//=============================== End of File ==================================
