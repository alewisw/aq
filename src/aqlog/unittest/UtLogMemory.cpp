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

#include "AQLog.h"

#include "LogMemory.h"

#include "AQHeapMemory.h"




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
TEST_SUITE(UtLogMemory);

//------------------------------------------------------------------------------
TEST(given_MinimumSizeMemoryRegion_when_LogMemory_then_AllocationValid)
{
    AQHeapMemory mem(AQLOG_SHM_MINIMUM_SIZE);

    LogMemory logMem(mem);
    REQUIRE(logMem.aqMemory().baseAddress() == mem.baseAddress());
    REQUIRE(logMem.aqMemory().size() == AQLOG_SHM_MINIMUM_SIZE - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    REQUIRE(logMem.logLevelHashMemory().baseAddress() == (unsigned char *)mem.baseAddress() + AQLOG_SHM_MINIMUM_SIZE - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    REQUIRE(logMem.logLevelHashMemory().size() == AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
}

//------------------------------------------------------------------------------
TEST(given_OffsetSizeMemoryRegion_when_LogMemory_then_AllocationValid)
{
    AQHeapMemory mem(AQLOG_SHM_MINIMUM_SIZE + 3);

    LogMemory logMem(mem);
    REQUIRE(logMem.aqMemory().baseAddress() == mem.baseAddress());
    REQUIRE(logMem.aqMemory().size() == AQLOG_SHM_MINIMUM_SIZE - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    REQUIRE(logMem.logLevelHashMemory().baseAddress() == (unsigned char *)mem.baseAddress() + AQLOG_SHM_MINIMUM_SIZE - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    REQUIRE(logMem.logLevelHashMemory().size() == AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
}

//------------------------------------------------------------------------------
TEST(given_LargeMemoryRegion_when_LogMemory_then_ExtraMemoryAddedToLog)
{
    AQHeapMemory mem(AQLOG_SHM_MINIMUM_SIZE + 1024);

    LogMemory logMem(mem);
    REQUIRE(logMem.aqMemory().baseAddress() == mem.baseAddress());
    REQUIRE(logMem.aqMemory().size() == AQLOG_SHM_MINIMUM_SIZE + 1024 - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    REQUIRE(logMem.logLevelHashMemory().baseAddress() == (unsigned char *)mem.baseAddress() + AQLOG_SHM_MINIMUM_SIZE + 1024 - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    REQUIRE(logMem.logLevelHashMemory().size() == AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
}

//------------------------------------------------------------------------------
TEST(given_MemoryRegionTooSmall_when_LogMemory_then_LengthErrorException)
{
    AQHeapMemory mem(AQLOG_SHM_MINIMUM_SIZE - 1);

    REQUIRE_EXCEPTION(LogMemory logMem(mem), length_error);
}



//=============================== End of File ==================================
