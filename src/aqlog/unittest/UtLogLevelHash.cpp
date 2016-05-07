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

#include "HashData.h"
#include "UtLogLevelHash.h"




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// Returns the tier zero offset for an item at index 'i0'.
#define OFF0(i0)        ((i0) << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM))

// Returns the tier one zero offset for an item at tier zero 'i0' and tier 1 'i1'.
#define OFF1(i0, i1)    (OFF0(i0) + ((i1) << (AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM)))

// The size of a tier zero region.
#define SZ0         (1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM))

// The size of a tier one region.
#define SZ1         (1 << (AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM))




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Checks the specified hash region to have the given level.
static void checkRegion(uint32_t off, uint32_t count, AQLogLevel_t level);

// Checks that an entry has been written into the hash at offset 'off', then 'off + skip' up to 'max'.
// The entry has the specified level.  The number of words to check in each region is given by size.
static void checkSkipRegion(uint32_t off, uint32_t skip, uint32_t max, AQLogLevel_t level, uint32_t size = 1);

// Takes a copy of the hash memory state for later comparison.
class HashComparer
{
public:

    HashComparer(void)
    {
        m_cmpHash = new uint32_t[AQLOG_HASH_TABLE_WORDS];
        memcpy(m_cmpHash, LogLevelHashTable_g, AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
    }

    ~HashComparer(void)
    {
        delete[] m_cmpHash;
    }

    bool matches(void)
    {
        return memcmp(m_cmpHash, LogLevelHashTable_g, AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t)) == 0;
    }

private:
    // The comparison hash data.
    uint32_t *m_cmpHash;

};




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE_FIRST(UtLogLevelHash);

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_Record_then_AcceptOnlyCritical)
{
    LogLevelHash hash(LogLevelHashTable_g);

    checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_CRITICAL);
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_CRITICAL, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_ERROR, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_CRITICAL, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_WARNING, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_0_B));
}

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_AddInfoFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO);
        hash.addHandler(&h1);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO);
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_AddDetailFilter_then_AcceptAtMostDetail)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_DETAIL);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_DETAIL);
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_AddNoticeFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO);
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_AddComponentInfoFilter_then_AcceptAtMostInfoForComponent)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_97_A);
        hash.addHandler(&h1);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, OFF0(97), AQLOG_LEVEL_CRITICAL);
        checkRegion(OFF0(97), SZ0, AQLOG_LEVEL_INFO);
        checkRegion(OFF0(97) + SZ0, AQLOG_HASH_TABLE_WORDS - OFF0(97) - SZ0, AQLOG_LEVEL_CRITICAL);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_TRACE, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_97_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddNoticeFilter_then_AcceptAtMostInfoForComponent)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_62_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(OFF0(62), SZ0, AQLOG_LEVEL_INFO);
        checkRegion(0, OFF0(62), AQLOG_LEVEL_NOTICE);
        checkRegion(OFF0(62) + SZ0, AQLOG_HASH_TABLE_WORDS - OFF0(62) - SZ0, AQLOG_LEVEL_NOTICE);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_62_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_62_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddDetailFilter_then_AcceptAtMostDetail)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_61_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_DETAIL);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_DETAIL);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_61_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_61_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddComponentNotice_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_15_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE, HASHIDX_15_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(OFF0(15), SZ0, AQLOG_LEVEL_INFO);
        checkRegion(0, OFF0(15), AQLOG_LEVEL_CRITICAL);
        checkRegion(OFF0(15) + SZ0, AQLOG_HASH_TABLE_WORDS - OFF0(15) - SZ0, AQLOG_LEVEL_CRITICAL);

        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_16_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_15_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_15_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_14_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentNoticeFilter_when_AddInfoNotice_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_NOTICE, HASHIDX_111_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_INFO, HASHIDX_111_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(OFF0(111), SZ0, AQLOG_LEVEL_INFO);
        checkRegion(0, OFF0(111), AQLOG_LEVEL_CRITICAL);
        checkRegion(OFF0(111) + SZ0, AQLOG_HASH_TABLE_WORDS - OFF0(111) - SZ0, AQLOG_LEVEL_CRITICAL);

        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_111_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_111_B, HASHIDX_15_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_15_B, HASHIDX_0_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentANoticeFilter_when_AddComponentBInfo_then_AcceptAtMostNoticeAInfoB)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_NOTICE, HASHIDX_97_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_INFO, HASHIDX_11_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, OFF0(11), AQLOG_LEVEL_CRITICAL);
        checkRegion(OFF0(11), SZ0, AQLOG_LEVEL_INFO);
        checkRegion(OFF0(11) + SZ0, OFF0(97) - (OFF0(11) + SZ0), AQLOG_LEVEL_CRITICAL);
        checkRegion(OFF0(97), SZ0, AQLOG_LEVEL_NOTICE);
        checkRegion(OFF0(97) + SZ0, AQLOG_HASH_TABLE_WORDS - OFF0(97) - SZ0, AQLOG_LEVEL_CRITICAL);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_11_B, HASHIDX_108_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_AddTagInfoFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", HASHIDX_33_A);
        hash.addHandler(&h1);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, OFF1(0, 33), AQLOG_LEVEL_CRITICAL);
        checkSkipRegion(OFF1(0, 33), SZ0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO);
        checkSkipRegion(OFF1(0, 34), SZ0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_CRITICAL, SZ0 - (OFF1(0, 34) - OFF1(0, 33)));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, HASHIDX_33_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_97_B, HASHIDX_33_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_32_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_AddTagInfoFilter_when_AddNoticeFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", HASHIDX_100_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, OFF1(0, 100), AQLOG_LEVEL_NOTICE);
        checkSkipRegion(OFF1(0, 100), SZ0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO, SZ1);
        checkSkipRegion(OFF1(0, 101), SZ0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_NOTICE, SZ0 - (OFF1(0, 101) - OFF1(0, 100)));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, HASHIDX_100_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_99_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_97_B, HASHIDX_101_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_97_B, HASHIDX_99_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_97_B, HASHIDX_101_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_TagInfoFilter_when_AddDetailFilter_then_AcceptAtMostDetail)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", HASHIDX_1_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_DETAIL);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_DETAIL);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_97_B, HASHIDX_1_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_97_B, HASHIDX_1_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_97_B, HASHIDX_2_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_97_B, HASHIDX_2_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_AddTagInfoFilter_when_AddComponentDetailFilter_then_AcceptAtMostDetailForComponent)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", HASHIDX_98_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_DETAIL, HASHIDX_34_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, OFF1(0, 98), AQLOG_LEVEL_CRITICAL);
        checkSkipRegion(OFF1(0, 98), SZ0, OFF0(34), AQLOG_LEVEL_INFO, SZ1);
        checkRegion(OFF0(34), SZ0, AQLOG_LEVEL_DETAIL);
        checkSkipRegion(OFF1(35, 98), SZ0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO, SZ1);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_98_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_99_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_33_B, HASHIDX_98_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_34_B, HASHIDX_98_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentDetailFilter_when_AddTagInfoFilter_then_AcceptAtMostDetailForComponent)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_DETAIL, HASHIDX_34_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_INFO, "", HASHIDX_98_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        checkRegion(0, OFF1(0, 98), AQLOG_LEVEL_CRITICAL);
        checkSkipRegion(OFF1(0, 98), SZ0, OFF0(34), AQLOG_LEVEL_INFO, SZ1);
        checkRegion(OFF0(34), SZ0, AQLOG_LEVEL_DETAIL);
        checkSkipRegion(OFF1(35, 98), SZ0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO, SZ1);

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_98_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_99_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_33_B, HASHIDX_98_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_34_B, HASHIDX_98_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddTagDetailFilter_then_AcceptAtMostDetailForTag)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_34_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_DETAIL, "", HASHIDX_98_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_34_B, "", HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, "", HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_34_B, HASHIDX_97_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_97_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_98_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_34_B, HASHIDX_98_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_34_B, HASHIDX_99_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_34_B, HASHIDX_99_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagDetail_when_AddTagInfoFilter_then_AcceptAtMostDetailForTag)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_DETAIL, HASHIDX_4_A, HASHIDX_8_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_INFO, HASHIDX_4_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_4_B, HASHIDX_7_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_4_B, HASHIDX_7_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_4_B, HASHIDX_8_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_4_B, HASHIDX_8_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_4_B, HASHIDX_9_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_4_B, HASHIDX_9_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_TagInfoFilter_when_AddComponentTagDetail_then_AcceptAtMostDetailForTag)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_4_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_DETAIL, HASHIDX_4_A, HASHIDX_8_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_4_B, HASHIDX_7_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_4_B, HASHIDX_7_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_4_B, HASHIDX_8_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_4_B, HASHIDX_8_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_4_B, HASHIDX_9_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_4_B, HASHIDX_9_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_TagDetailFilter_when_AddComponentNoticeAndTagInfo_then_AcceptAtMostDetailForTag)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_DETAIL, "", HASHIDX_48_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE, HASHIDX_13_A);
        hash.addHandler(&h2);
        TestHandler h3(AQLOG_LEVEL_INFO, HASHIDX_13_A, HASHIDX_109_A);
        hash.addHandler(&h3);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_12_B, HASHIDX_48_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_12_B, HASHIDX_48_B, HASHIDX_2_B));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_13_B, HASHIDX_48_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_13_B, HASHIDX_48_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_13_B, HASHIDX_49_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_13_B, HASHIDX_49_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_13_B, HASHIDX_109_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_13_B, HASHIDX_109_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagInfoFilter_when_AddComponentNoticeAndTagDetail_then_AcceptAtMostDetailForTag)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_13_A, HASHIDX_109_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE, HASHIDX_13_A);
        hash.addHandler(&h2);
        TestHandler h3(AQLOG_LEVEL_DETAIL, "", HASHIDX_48_A);
        hash.addHandler(&h3);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_12_B, HASHIDX_48_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_12_B, HASHIDX_48_B, HASHIDX_2_B));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_13_B, HASHIDX_48_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DEBUG, HASHIDX_13_B, HASHIDX_48_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_13_B, HASHIDX_49_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_13_B, HASHIDX_49_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_13_B, HASHIDX_109_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_13_B, HASHIDX_109_B, HASHIDX_2_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_AddFileInfoFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", "", HASHIDX_6_A);
        hash.addHandler(&h1);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, "", HASHIDX_6_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_96_B, "", HASHIDX_6_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_ERROR, HASHIDX_96_B, "", HASHIDX_5_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_ERROR, HASHIDX_96_B, "", HASHIDX_7_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_FileInfoFilter_when_AddComponentNoticeFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", "", HASHIDX_6_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE, HASHIDX_96_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, "", HASHIDX_6_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_96_B, "", HASHIDX_6_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_96_B, "", HASHIDX_5_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, "", HASHIDX_5_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_96_B, "", HASHIDX_4_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, "", HASHIDX_4_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_FileInfoFilter_when_AddTagNoticeFilter_then_AcceptAtMostInfo)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, "", "", HASHIDX_6_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE, "", HASHIDX_24_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, HASHIDX_24_B, HASHIDX_6_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_96_B, HASHIDX_24_B, HASHIDX_6_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_96_B, HASHIDX_24_B, HASHIDX_5_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, HASHIDX_24_B, HASHIDX_5_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_96_B, HASHIDX_24_B, HASHIDX_4_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_96_B, HASHIDX_24_B, HASHIDX_4_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_ComponentNoticeFilter_when_AddTagFileInfoFilter_then_AcceptInfoForThatFile)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_NOTICE, HASHIDX_101_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_INFO, HASHIDX_101_A, HASHIDX_72_A, HASHIDX_3_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_3_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_3_B));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_4_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_4_B));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_71_B, HASHIDX_3_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_71_B, HASHIDX_3_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_73_B, HASHIDX_3_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_73_B, HASHIDX_3_B));
    }
}

//------------------------------------------------------------------------------
TEST(given_TagFileInfoFilter_when_AddComponentNoticeFilter_then_AcceptInfoForThatFile)
{
    for (size_t i = 0; i < 2; ++i)
    {
        LogLevelHash hash(LogLevelHashTable_g);
        if (i == 1) { hash.freezeHash(); }
        TestHandler h1(AQLOG_LEVEL_INFO, HASHIDX_101_A, HASHIDX_72_A, HASHIDX_3_A);
        hash.addHandler(&h1);
        TestHandler h2(AQLOG_LEVEL_NOTICE, HASHIDX_101_A);
        hash.addHandler(&h2);
        if (i == 1) { hash.unfreezeHash(); }

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_3_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_3_B));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_2_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_2_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_4_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_72_B, HASHIDX_4_B));

        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_71_B, HASHIDX_3_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_71_B, HASHIDX_3_B));
        REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, HASHIDX_101_B, HASHIDX_73_B, HASHIDX_3_B));
        REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, HASHIDX_101_B, HASHIDX_73_B, HASHIDX_3_B));
    }
}

//------------------------------------------------------------------------------
static void checkRegion(uint32_t off, uint32_t count, AQLogLevel_t level)
{
    uint32_t m = (level << 28)
        | (level << 24)
        | (level << 20)
        | (level << 16)
        | (level << 12)
        | (level << 8)
        | (level << 4)
        | (level << 0);
    for (size_t i = off; i < off + count; ++i)
    {
        if (AQLog_LevelHashTable_g[i] != m)
        {
            CHECK(AQLog_LevelHashTable_g[i] == m);
            ostringstream ss;
            ss << endl << "Index " << i << ": " << hex << setw(8) << setfill('0') << AQLog_LevelHashTable_g[i] << endl;
            cout << ss.str();
            return;
        }
    }
}

//------------------------------------------------------------------------------
static void checkSkipRegion(uint32_t off, uint32_t skip, uint32_t max, AQLogLevel_t level, uint32_t size)
{
    uint32_t m = (level << 28)
        | (level << 24)
        | (level << 20)
        | (level << 16)
        | (level << 12)
        | (level << 8)
        | (level << 4)
        | (level << 0);
    for (size_t i = off; i < max; i += skip)
    {
        for (size_t j = 0; j < size && i + j < max; ++j)
        {
            if (AQLog_LevelHashTable_g[i + j] != m)
            {
                CHECK(AQLog_LevelHashTable_g[i + j] == m);
                ostringstream ss;
                ss << endl << "Index " << (i + j) << ": " << hex << setw(8) << setfill('0') << AQLog_LevelHashTable_g[i] << endl;
                cout << ss.str();
                return;
            }
        }
    }
}




//=============================== End of File ==================================
