#ifndef LOGREADERTEST_H
#define LOGREADERTEST_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLog.h"

#include "LogLevelHash.h"
#include "LogMemory.h"
#include "LogReader.h"

#include "AQExternMemory.h"
#include "AQWriter.h"

#include "TestHandler.h"

#include <string>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogRecord;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Provides the environment to execute a log reader test.
class LogReaderTest
{
public:

    // Constructs the objects needed to run a log reader test.
    LogReaderTest(AQLogLevel_t level, std::string tier1 = "",
        std::string tier2 = "", std::string tier3 = "");

    // Destroys the objects needed to run a log reader test.
    ~LogReaderTest(void);

    // Returns the next record from this reader given that it is empty.
    AQLogRecord *nextRecordAfterEmpty(void);

    // Returns the next record from this reader that has the specified level or
    // NULL if none is found.
    AQLogRecord *nextLevelRecord(AQLogLevel_t level);

    // Requires that the header for the passed record (everything except data 
    // and message string) match the passed fields.
    //
    // Note:
    //  line - If 0 then no test is performed.
    //  timestampNs - If 0 then it just checks that the timestamp is in the past.
    //  processName - If NULL then all process data is compared to the current
    //                ProcessIdentifier data.
    void requireHeader(const AQLogRecord *rec, const char *componentId,
        const char *tagId, const char *file, uint32_t line, const char *function,
        AQLogLevel_t level, uint64_t timestampNs = 0, const char *processName = NULL,
        uint32_t processId = 0, uint32_t threadId = 0);

    // Verifies the passed record that only contains a text message.  The 
    // message must be complete.
    void requireMessage(const AQLogRecord *rec, const char *message,
        const char *componentId, const char *tagId, const char *file,
        uint32_t line, const char *function, AQLogLevel_t level,
        uint64_t timestampNs = 0, const char *processName = NULL,
        uint32_t processId = 0, uint32_t threadId = 0);

    // Verifies the passed record that only contains a text message.  The 
    // message must be truncated.
    void requireTruncatedMessage(const AQLogRecord *rec, const char *message,
        const char *componentId, const char *tagId, const char *file,
        uint32_t line, const char *function, AQLogLevel_t level,
        uint64_t timestampNs = 0, const char *processName = NULL,
        uint32_t processId = 0, uint32_t threadId = 0);

    // Verifies the passed record that contains data and a text message.
    // The message and data must be complete.
    void requireData(const AQLogRecord *rec, const char *message,
        const void *data, size_t dataSize, const char *componentId,
        const char *tagId, const char *file, uint32_t line,
        const char *function, AQLogLevel_t level, uint64_t timestampNs = 0,
        const char *processName = NULL, uint32_t processId = 0,
        uint32_t threadId = 0);

    // Verifies the passed record that contains data and a text message.
    // The data must be truncated.
    void requireTruncatedData(const AQLogRecord *rec, const char *message,
        const void *data, const char *componentId, const char *tagId,
        const char *file, uint32_t line, const char *function,
        AQLogLevel_t level, uint64_t timestampNs = 0,
        const char *processName = NULL, uint32_t processId = 0,
        uint32_t threadId = 0);

    // Verifies the passed record that contains data and a text message.
    // The message and data must be truncated.
    void requireTruncatedMessageData(const AQLogRecord *rec, const char *message,
        const void *data, const char *componentId, const char *tagId,
        const char *file, uint32_t line, const char *function,
        AQLogLevel_t level, uint64_t timestampNs = 0,
        const char *processName = NULL, uint32_t processId = 0,
        uint32_t threadId = 0);

private:

    // The log memory used in the m_sm field.
    uint32_t m_mem[AQLOG_HASH_TABLE_WORDS + 10000];

    // The shared memory.
    AQExternMemory m_sm;

    // The log memory that divides the shared memory.
    LogMemory m_logMem;

    // The test handler to use to set the level.
    TestHandler m_handler;

public:

    // The log reader.
    LogReader reader;

    // The log level hash.
    LogLevelHash hash;

    // The log - used to access information about the log state.
    AQWriter aq;

};




#endif
//=============================== End of File ==================================
