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

#include "UtLogLevelHash.h"




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// 12 collisions
#define COLLTEST_A_1 "ehXhVjd"
#define COLLTEST_A_2 "esBhRlG"
#define COLLTEST_A_3 "RfqHAzS"
#define COLLTEST_A_4 "pGScDnH"
#define COLLTEST_A_5 "bgfWcTf"
#define COLLTEST_A_6 "Uwtieyf"
#define COLLTEST_A_7 "OMjbcma"
#define COLLTEST_A_8 "wgcCsdB"
#define COLLTEST_A_9 "VAOPSqO"
#define COLLTEST_A_10 "ZivPKLW"
#define COLLTEST_A_11 "MnJdMil"
#define COLLTEST_A_12 "xpmyfgx"

// 11 collisions
#define COLLTEST_B_1 "MwIyKHo"
#define COLLTEST_B_2 "bUxqzCA"
#define COLLTEST_B_3 "hliYazi"
#define COLLTEST_B_4 "XgoDiSH"
#define COLLTEST_B_5 "wlMasIk"
#define COLLTEST_B_6 "ofjbgff"
#define COLLTEST_B_7 "ihoWdlw"
#define COLLTEST_B_8 "APvxNzM"
#define COLLTEST_B_9 "TFYKrUA"
#define COLLTEST_B_10 "zwZShnd"
#define COLLTEST_B_11 "zAaImPH"

// 7 collisions
#define COLLTEST_C_1 "NzPzRjG"
#define COLLTEST_C_2 "QUdGcmh"
#define COLLTEST_C_3 "OkhjkZV"
#define COLLTEST_C_4 "yYPKEJU"
#define COLLTEST_C_5 "adJAbxU"
#define COLLTEST_C_6 "gdsDTVW"
#define COLLTEST_C_7 "GNEPzfa"

// 8 collisions
#define COLLTEST_D_1 "NyxuyvY"
#define COLLTEST_D_2 "HmezwiH"
#define COLLTEST_D_3 "xpxSqIw"
#define COLLTEST_D_4 "HrtoEFn"
#define COLLTEST_D_5 "psqdqXs"
#define COLLTEST_D_6 "PHhblKy"
#define COLLTEST_D_7 "NcCzdRn"
#define COLLTEST_D_8 "WfHRAWq"

// 9 collisions
#define COLLTEST_E_1 "JrTphxb"
#define COLLTEST_E_2 "hJGaTxf"
#define COLLTEST_E_3 "Wjpmrth"
#define COLLTEST_E_4 "AdhIlNh"
#define COLLTEST_E_5 "zUaJLGa"
#define COLLTEST_E_6 "sZvnBro"
#define COLLTEST_E_7 "imFUiUe"
#define COLLTEST_E_8 "VOGFPHN"
#define COLLTEST_E_9 "DKPyWxg"

// 4 collisions
#define COLLTEST_F_1 "QGTOgNL"
#define COLLTEST_F_2 "UBqmFDA"
#define COLLTEST_F_3 "TNtSvlo"
#define COLLTEST_F_4 "VJJtggj"




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Checks the specified hash region to have the given level.
static void checkRegion(uint32_t off, uint32_t count, AQLogLevel_t level);




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
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_CRITICAL, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_ERROR, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_CRITICAL, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_WARNING, "comp_bar", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_AddInfoFilter_then_AcceptAtMostInfo)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO);
    hash.addHandler(&h1);

    checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO);
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_AddDetailFilter_then_AcceptAtMostDetail)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO);
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_DETAIL);
    hash.addHandler(&h2);

    checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_DETAIL);
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_AddNoticeFilter_then_AcceptAtMostInfo)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO);
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_NOTICE);
    hash.addHandler(&h2);

    checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_INFO);
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_NoFilter_when_AddComponentInfoFilter_then_AcceptAtMostInfoForComponent)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo");
    hash.addHandler(&h1);

    size_t h = HashFunction::standard("comp_foo", 8) & AQLOG_TIER_0_MASK;
    size_t off = h << (AQLOG_TIER_0_BITNUM - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t count = 1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM);
    checkRegion(off, count, AQLOG_LEVEL_INFO);
    checkRegion(0, off, AQLOG_LEVEL_CRITICAL);
    checkRegion(off + count, AQLOG_HASH_TABLE_WORDS - off - count, AQLOG_LEVEL_CRITICAL);

    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_TRACE, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_foo", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddNoticeFilter_then_AcceptAtMostInfoForComponent)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo");
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_NOTICE);
    hash.addHandler(&h2);

    size_t h = HashFunction::standard("comp_foo", 8) & AQLOG_TIER_0_MASK;
    size_t off = h << (AQLOG_TIER_0_BITNUM - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t count = 1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM);
    checkRegion(off, count, AQLOG_LEVEL_INFO);
    checkRegion(0, off, AQLOG_LEVEL_NOTICE);
    checkRegion(off + count, AQLOG_HASH_TABLE_WORDS - off - count, AQLOG_LEVEL_NOTICE);

    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_foo", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddDetailFilter_then_AcceptAtMostDetail)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo");
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_DETAIL);
    hash.addHandler(&h2);

    checkRegion(0, AQLOG_HASH_TABLE_WORDS, AQLOG_LEVEL_DETAIL);

    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_foo", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_ComponentInfoFilter_when_AddComponentNotice_then_AcceptAtMostInfo)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar");
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_NOTICE, "comp_bar");
    hash.addHandler(&h2);

    size_t h = HashFunction::standard("comp_bar", 8) & AQLOG_TIER_0_MASK;
    size_t off = h << (AQLOG_TIER_0_BITNUM - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t count = 1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM);
    checkRegion(off, count, AQLOG_LEVEL_INFO);
    checkRegion(0, off, AQLOG_LEVEL_CRITICAL);
    checkRegion(off + count, AQLOG_HASH_TABLE_WORDS - off - count, AQLOG_LEVEL_CRITICAL);

    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_ComponentNoticeFilter_when_AddInfoNotice_then_AcceptAtMostInfo)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_NOTICE, "comp_bar");
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_INFO, "comp_bar");
    hash.addHandler(&h2);

    size_t h = HashFunction::standard("comp_bar", 8) & AQLOG_TIER_0_MASK;
    size_t off = h << (AQLOG_TIER_0_BITNUM - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t count = 1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM);
    checkRegion(off, count, AQLOG_LEVEL_INFO);
    checkRegion(0, off, AQLOG_LEVEL_CRITICAL);
    checkRegion(off + count, AQLOG_HASH_TABLE_WORDS - off - count, AQLOG_LEVEL_CRITICAL);

    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO, "comp_foo", "tag_bar", "file_bar"));
}

//------------------------------------------------------------------------------
TEST(given_ComponentANoticeFilter_when_AddComponentBInfo_then_AcceptAtMostNoticeAInfoB)
{
    LogLevelHash hash(LogLevelHashTable_g);
    TestHandler h1(AQLOG_LEVEL_NOTICE, "comp_foo");
    hash.addHandler(&h1);
    TestHandler h2(AQLOG_LEVEL_INFO, "comp_bar");
    hash.addHandler(&h2);

    size_t ha1 = HashFunction::standard("comp_foo", 8) & AQLOG_TIER_0_MASK;
    size_t off1 = ha1 << (AQLOG_TIER_0_BITNUM - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t count1 = 1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t ha2 = HashFunction::standard("comp_bar", 8) & AQLOG_TIER_0_MASK;
    size_t off2 = ha2 << (AQLOG_TIER_0_BITNUM - AQLOG_HASH_INDEX_WORD_BITNUM);
    size_t count2 = 1 << (AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM);
    CHECK(ha1 != ha2);

    if (off1 < off2)
    {
        checkRegion(0, off1, AQLOG_LEVEL_CRITICAL);
        checkRegion(off1, count1, AQLOG_LEVEL_NOTICE);
        checkRegion(off1 + count1, off1 + count1 - off2, AQLOG_LEVEL_CRITICAL);
        checkRegion(off2, count2, AQLOG_LEVEL_INFO);
        checkRegion(off2 + count2, AQLOG_HASH_TABLE_WORDS - off2 - count2, AQLOG_LEVEL_CRITICAL);
    }
    else
    {
        checkRegion(0, off2, AQLOG_LEVEL_CRITICAL);
        checkRegion(off2, count2, AQLOG_LEVEL_NOTICE);
        checkRegion(off2 + count2, off2 + count2 - off1, AQLOG_LEVEL_CRITICAL);
        checkRegion(off1, count1, AQLOG_LEVEL_INFO);
        checkRegion(off1 + count1, AQLOG_HASH_TABLE_WORDS - off1 - count1, AQLOG_LEVEL_CRITICAL);
    }

    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_NOTICE,  "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO,   "comp_foo", "tag_foo", "file_foo"));
    REQUIRE(AQLOG_HASHISLEVEL(AQLOG_LEVEL_INFO,    "comp_bar", "tag_foo", "file_foo"));
    REQUIRE(!AQLOG_HASHISLEVEL(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_foo", "file_foo"));
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
            break;
        }
    }
}




//=============================== End of File ==================================
