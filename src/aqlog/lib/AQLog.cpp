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

#include "AQLogRecord.h"

#include "HashFunction.h"
#include "LogMemory.h"
#include "Atomic.h"
#include "ProcessIdentifier.h"
#include "Timestamp.h"

#include "AQWriter.h"
#include "AQWriterItem.h"

#include <stdexcept>

using namespace aqlog;
using namespace aqosa;
using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// The mask used to set the lost flag bit.
#define LOST_FLAG_MASK                  (1 << 0)



//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Calculates the hash value of a string str_ consisting of strLen characters
// in the string.  The hash is returned.
static AQLOG_HASH_EXTERN_ATTRIBUTE uint32_t AQLog_HashTierExtern(const char *str, size_t strLen);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

const uint32_t *AQLog_LevelHashTable_g;

// The identifier of the current process.
static uint32_t ProcessId = 0;

// The name of the current process.
static string ProcessName;

// The region of shared memory used by the log.
static LogMemory *SharedMemory = NULL;

// The allocating queue used to write messages into the log.
static AQWriter *Writer = NULL;

// Set bit 0 to '1' when we have lost a record and not yet report it.
static uint32_t LostFlag = 0;




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
extern "C" AQLOG_HASH_EXTERN_ATTRIBUTE bool AQLog_HashIsLevelExtern(int level,
    const char *str1, size_t str1Size, const char *str2, size_t str2Size, 
    const char *str3, size_t str3Size)
{
    uint32_t tier0Hash = HashFunction::standard(AQLOG_TIER_0_MASK, str1, str1Size, AQLOG_LOOKUP_TIER_TAGID == 0);
    uint32_t tier1Hash = HashFunction::standard(AQLOG_TIER_1_MASK, str2, str2Size, AQLOG_LOOKUP_TIER_TAGID == 1);
    uint32_t tier2Hash = HashFunction::standard(AQLOG_TIER_2_MASK, str3, str3Size, AQLOG_LOOKUP_TIER_TAGID == 2);

    uint32_t index = (tier0Hash << AQLOG_TIER_0_BITNUM)
        | (tier1Hash << AQLOG_TIER_1_BITNUM)
        | (tier2Hash << AQLOG_TIER_2_BITNUM);
    uint32_t word = index >> AQLOG_HASH_INDEX_WORD_BITNUM;
    uint32_t bitnum = (index & AQLOG_HASH_INDEX_LEVEL_MASK) << AQLOG_HASH_LEVEL_BITS_MUL_SHIFT;

    return (AQLog_LevelHashTable_g[word] & (AQLOG_HASH_LEVEL_MASK << bitnum)) >= ((uint32_t)level << bitnum);
}

//------------------------------------------------------------------------------
extern "C" AQLogInitOutcome_t AQLog_InitSharedMemory(IAQSharedMemory& sm)
{
    LogMemory *sharedMemory;

    // Allocate the new shared memory region.
    try
    {
        sharedMemory = new LogMemory(sm);
    }
    catch (const length_error&)
    {
        return AQLOG_INITOUTCOME_SHM_TOO_SMALL;
    }

    // Next try to create the writer.
    AQWriter *writer = new AQWriter(sharedMemory->aqMemory());
    if (!writer->isFormatted())
    {
        delete sharedMemory;
        return AQLOG_INITOUTCOME_AQ_UNFORMATTED;
    }

    // Success; release any current configuration.
    AQLog_Deinit();

    // Setup local copies of static information (saves syscalls during logging).
    ProcessId = ProcessIdentifier::currentProcessId();
    ProcessName = ProcessIdentifier::currentProcessName();

    // Now assign the new configuration.
    SharedMemory = sharedMemory;
    Writer = writer;
    AQLog_LevelHashTable_g = (uint32_t *)sharedMemory->logLevelHashMemory().baseAddress();

    return AQLOG_INITOUTCOME_SUCCESS;
}

//------------------------------------------------------------------------------
extern "C" void AQLog_Deinit(void)
{
    if (Writer != NULL)
    {
        delete Writer;
        Writer = NULL;
    }
    if (SharedMemory != NULL)
    {
        delete SharedMemory;
        SharedMemory = NULL;
    }
    AQLog_LevelHashTable_g = NULL;
}

//------------------------------------------------------------------------------
extern "C" void __AQLog_Write(AQLogLevel_t level, const char *componentId,
    size_t componentIdSize, const char *tagId, size_t tagIdSize,
    const char *file, size_t fileSize, const char *func, size_t funcSize,
    int line, const void *data, size_t dataSize, const char *msg, ...)
{
    AQWriterItem item;

    /* Adjust so that we just get the filename. */
    size_t i = fileSize;
    while (i > 0 && !(file[i - 1] == '/' || file[i - 1] == '\\'))
    {
        i--;
    }
    if (i > 0)
    {
        file = &file[i];
        fileSize -= i;
    }

    size_t reqSize = sizeof(AQLogRecord::Overlay)
        + dataSize
        + ProcessName.size() + 1
        + componentIdSize
        + tagIdSize
        + fileSize
        + funcSize
        + AQLOG_RESERVE_MESSAGE_SIZE;

    bool dataTruncated = false;
    if (!Writer->claim(item, reqSize))
    {
        // Could not allocate - if we had data try to truncate it and retry.
        if (dataSize <= AQLOG_DATA_TRUNCATE_SIZE)
        {
            // Out of queue space - drop the log message.
            Atomic::bitwiseOr(&LostFlag, LOST_FLAG_MASK);
            return;
        }
        else
        {
            reqSize = reqSize - dataSize + AQLOG_DATA_TRUNCATE_SIZE;
            dataSize = AQLOG_DATA_TRUNCATE_SIZE;
            dataTruncated = true;
            if (!Writer->claim(item, reqSize))
            {
                // Out of queue space even with data truncation enabled - drop the log message.
                Atomic::bitwiseOr(&LostFlag, LOST_FLAG_MASK);
                return;
            }
        }
    }

    // Use the overlay to construct the record.
    AQLogRecord::Overlay *rec = (AQLogRecord::Overlay *)&item[0];
    rec->dropped = Atomic::read(&LostFlag) & LOST_FLAG_MASK;
    if (rec->dropped)
    {
        Atomic::bitwiseAnd(&LostFlag, (uint32_t)~LOST_FLAG_MASK);
    }
    rec->truncatedStr = 0;
    rec->truncatedData = dataTruncated ? 1 : 0;
    rec->reservedFlag = 0;
    rec->logLevel = level;
    rec->lineNumber = (uint32_t)line;

    rec->processId = ProcessId;
    rec->threadId = ProcessIdentifier::currentThreadId();
    rec->timestampNs = Timestamp::now();

    rec->dataSize = (uint32_t)dataSize;

    // Insert the strings - due to the allocated size there must be enough
    // space.
    char *ptr = &rec->strData[0];
    if (dataSize > 0)
    {
        memcpy(ptr, data, dataSize);
        ptr += dataSize;
    }
    
    memcpy(ptr, componentId, componentIdSize);
    ptr += componentIdSize;

    memcpy(ptr, tagId, tagIdSize);
    ptr += tagIdSize;

    memcpy(ptr, file, fileSize);
    ptr += fileSize;

    memcpy(ptr, ProcessName.c_str(), ProcessName.size() + 1);
    ptr += ProcessName.size() + 1;

    memcpy(ptr, func, funcSize);
    ptr += funcSize;

    // Now the message - this could fail as we didn't know the length
    // up-front.
    va_list argp;
    va_start(argp, msg);
    if (item.vprintf((size_t)((uintptr_t)ptr - (uintptr_t)rec), msg, argp) < 0)
    {
        rec->truncatedStr = 1;
    }
    va_end(argp);

    if (!Writer->commit(item))
    {
        // Commit error - set the lost mask.
        Atomic::bitwiseOr(&LostFlag, LOST_FLAG_MASK);
    }
}




//=============================== End of File ==================================
