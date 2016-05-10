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

#include "AQLog.h"
#include "HashFunction.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif




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
TEST_SUITE(UtHashFunction);

//------------------------------------------------------------------------------
TEST(given_HashFunction_when_Hash_then_FindUnique)
{
    uint32_t maxTierMask = AQLOG_TIER_0_MASK;
    const char *maxTierName = "AQLOG_TIER_0_MASK";
    if (AQLOG_TIER_1_MASK > maxTierMask)
    {
        maxTierMask = AQLOG_TIER_1_MASK;
        maxTierName = "AQLOG_TIER_1_MASK";
    }
    if (AQLOG_TIER_2_MASK > maxTierMask)
    {
        maxTierMask = AQLOG_TIER_2_MASK;
        maxTierName = "AQLOG_TIER_2_MASK";
    }

    set<uint32_t> pending;
    for (size_t i = 0; i <= maxTierMask; ++i)
    {
        pending.insert(i);
    }

    set<string> *match = new set<string>[maxTierMask + 1];
    HashFunction_fn hashFn = HashFunction::standard;
    string matchZeroZero;
    while (pending.size() > 0 || matchZeroZero.size() == 0)
    {
        ostringstream ss;
        size_t len = 5 + rand() % 6;

        for (size_t j = 0; j < len; ++j)
        {
            char ch = rand() % 26;
            if (rand() & 1)
            {
                ch += 'A';
            }
            else
            {
                ch += 'a';
            }
            ss << ch;
        }
        string s = ss.str();
        uint32_t hash = hashFn(maxTierMask, s.c_str(), s.size(), false);

        if (hash == 0 && (s[s.size() - 1] & 0x0F) == 0)
        {
            matchZeroZero = s;
        }
        
        match[hash].insert(s);
        if (match[hash].size() >= 3)
        {
            pending.erase(hash);
        }
    }

    char str[1024];
    string dir;
#ifdef _WIN32
    GetCurrentDirectoryA(sizeof(str), str);
    str[sizeof(str) - 1] = '\0';
    dir += str;
    dir += "\\";
#else
    getcwd(str, sizeof(str));
    str[sizeof(str) - 1] = '\0';
    dir += str;
    dir += "/";
#endif
    dir += "HashData.raw";

    cout << "Writing hash data to: " << dir << endl;
    ofstream file(dir.c_str());

    // Generate the output.
    size_t count = 0;
    file << "#define HASHIDX_0_MATCH00_MASKxF \"" << matchZeroZero << "\"" << endl;
    for (uint32_t i = 0; i <= maxTierMask; ++i)
    {
        set<string>& v = match[i];
        char ch = 'A';
        for (set<string>::iterator it = v.begin(); it != v.end() && ch <= 'C'; ++it, ++ch)
        {
            file << "#define HASHIDX_" << i << "_" << ch << " \"" << (*it) << "\"" << endl;
            count++;
            if (i == 0)
            {
                uint32_t rehash = hashFn(maxTierMask, (*it).c_str(), (*it).size(), true);
                file << "#define HASHIDX_" << i << "_" << ch << "_REHASH_IDX " << rehash << endl;
            }
        }
    }

    // Generate the search table.
    file << "#define HASHIDX_TABLE_COUNT " << count << endl;
    file << "const char *const HashIdxTable_g[HASHIDX_TABLE_COUNT] = " << endl;
    file << "{" << endl;
    for (uint32_t i = 0; i <= maxTierMask; ++i)
    {
        set<string>& v = match[i];
        char ch = 'A';
        for (set<string>::iterator it = v.begin(); it != v.end() && ch <= 'C'; ++it, ++ch)
        {
            file << "    {HASHIDX_" << i << "_" << ch << "}," << endl;
        }
    }
    file << "};" << endl;

    // Generate the test case.
    file << endl;
    file << "//------------------------------------------------------------------------------" << endl;
    file << "TEST(given_HashIdxConstants_when_Hashed_then_IndexesMatch)" << endl;
    file << "{" << endl;
    file << "    REQUIRE(1 == (HashFunction::standard(0xF, HASHIDX_0_MATCH00_MASKxF,  sizeof(HASHIDX_0_MATCH00_MASKxF) - 1, true)));" << endl;
    for (uint32_t i = 0; i <= maxTierMask; ++i)
    {
        set<string>& v = match[i];
        char ch = 'A';
        for (set<string>::iterator it = v.begin(); it != v.end() && ch <= 'C'; ++it, ++ch)
        {
            file << "    REQUIRE(" << i << " == (HashFunction::standard(" << maxTierName << ", HASHIDX_" << i << "_" << ch << ",  sizeof(HASHIDX_" << i << "_" << ch << ") - 1, false)));" << endl;
            if (i == 0)
            {
                file << "    REQUIRE(HASHIDX_" << i << "_" << ch << "_REHASH_IDX == (HashFunction::standard(" << maxTierName << ", HASHIDX_" << i << "_" << ch << ",  sizeof(HASHIDX_" << i << "_" << ch << ") - 1, true)));" << endl;
            }
        }
    }
    file << "}" << endl;
}

//------------------------------------------------------------------------------
TEST(given_String_when_InlineAndExternHashCalculated_then_HashesMatch)
{
    vector<string> strings;

    strings.push_back("");          // empty
    strings.push_back("1");         // single character
    strings.push_back("aabbcc");    // lower case characters
    strings.push_back("AABBCC");    // upper case characters
    strings.push_back("AaBbCc");    // mixed case characters

    ostringstream ss;
    for (size_t i = 0; i < AQLOG_HASH_CHARMAX - 1; ++i)
    {
        ss << (char)('A' + i);
    }
    strings.push_back(ss.str());    // maximum length take 1
    ss << '1';
    strings.push_back(ss.str());    // maximum length
    ss << '2';
    strings.push_back(ss.str());    // maximum length plus 1

                                    // Slashes
    ss << "/1";
    strings.push_back(ss.str());    // slash
    for (size_t i = 0; i < AQLOG_HASH_CHARMAX - 3; ++i)
    {
        ss << (char)('A' + i);
    }
    strings.push_back(ss.str());    // slash with more characters
    ss << "\\1";
    strings.push_back(ss.str());    // backslash
    for (size_t i = 0; i < AQLOG_HASH_CHARMAX - 3; ++i)
    {
        ss << (char)('A' + i);
    }
    strings.push_back(ss.str());    // backslash with more characters

    for (size_t i = 0; i < strings.size(); ++i)
    {
        uint32_t inlineHash;
        AQLOG_HASHTIER_INLINE(inlineHash, UINT32_MAX, strings[i].c_str(), strings[i].size(), false);

        uint32_t externHash = HashFunction::standard(UINT32_MAX, strings[i].c_str(), strings[i].size(), false);

        cout << "Hashing \"" << strings[i] << "\"" << endl;
        REQUIRE(inlineHash == externHash);
    }
}

//------------------------------------------------------------------------------
TEST(given_MatchZeroZeroString_when_InlineAndExternHashCalculated_then_HashesMatch)
{
    uint32_t inlineHash;
    AQLOG_HASHTIER_INLINE(inlineHash, 0xF, HASHIDX_0_MATCH00_MASKxF, sizeof(HASHIDX_0_MATCH00_MASKxF) - 1, true);

    REQUIRE(HashFunction::standard(0xF, HASHIDX_0_MATCH00_MASKxF, sizeof(HASHIDX_0_MATCH00_MASKxF) - 1, true)
        == inlineHash);
}

//------------------------------------------------------------------------------
TEST(given_ReplaceZeroFalse_when_InlineAndExternHashCalculated_then_ZeroHashesMatch)
{
    uint32_t inlineHash;
    AQLOG_HASHTIER_INLINE(inlineHash, AQLOG_TIER_0_MASK, HASHIDX_0_A, sizeof(HASHIDX_0_A) - 1, false);

    REQUIRE(inlineHash == 0);
    REQUIRE(HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_A, sizeof(HASHIDX_0_A) - 1, false)
        == inlineHash);
}

//------------------------------------------------------------------------------
TEST(given_ReplaceZeroTrue_when_InlineAndExternHashCalculated_then_NonZeroHashesMatch)
{
    uint32_t inlineHash;
    AQLOG_HASHTIER_INLINE(inlineHash, AQLOG_TIER_0_MASK, HASHIDX_0_A, sizeof(HASHIDX_0_A) - 1, true);

    REQUIRE(inlineHash != 0);
    REQUIRE(HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_A, sizeof(HASHIDX_0_A) - 1, true)
        == inlineHash);
}

//------------------------------------------------------------------------------
TEST(given_ReplaceZeroTrue_when_InlineAndExternHashCalculatedOnZeroLengthString_then_ZeroHashesMatch)
{
    uint32_t inlineHash;
    AQLOG_HASHTIER_INLINE(inlineHash, AQLOG_TIER_0_MASK, "", 0, true);

    REQUIRE(inlineHash == 0);
    REQUIRE(HashFunction::standard(AQLOG_TIER_0_MASK, "", 0, true)
        == inlineHash);
}

//------------------------------------------------------------------------------
TEST(given_SlashInString_when_HashCalculated_then_OnlyPartAfterSlashConsidered)
{
#define STR1 "sdfasdf/w33234"
#define STR2 "546gd2f/w33234"
    REQUIRE(   HashFunction::standard(UINT32_MAX, STR1, sizeof(STR1) - 1, false) 
            == HashFunction::standard(UINT32_MAX, STR2, sizeof(STR2) - 1, false));
#undef STR1
#undef STR2
}

//------------------------------------------------------------------------------
TEST(given_BackslashInString_when_HashCalculated_then_OnlyPartAfterBackslashConsidered)
{
#define STR1 "sdfds1f\\633234"
#define STR2 "546gs2f\\633234"
    REQUIRE(HashFunction::standard(UINT32_MAX, STR1, sizeof(STR1) - 1, false)
        == HashFunction::standard(UINT32_MAX, STR2, sizeof(STR2) - 1, false));
#undef STR1
#undef STR2
}

//------------------------------------------------------------------------------
TEST(given_StringLargerMaxChars_when_HashCalculated_then_LastMaxCharsConsidered)
{
    ostringstream s1;
    ostringstream s2;

    s1 << "X";
    for (size_t i = 0; i < AQLOG_HASH_CHARMAX; ++i)
    {
        s1 << (char)('A' + i);
        s2 << (char)('A' + i);
    }
    REQUIRE(HashFunction::standard(UINT32_MAX, s1.str().c_str(), s1.str().size(), false) == HashFunction::standard(UINT32_MAX, s2.str().c_str(), s2.str().size(), false));
}

//------------------------------------------------------------------------------
TEST(given_HashIdxConstants_when_Hashed_then_IndexesMatch)
{
    REQUIRE(1 == (HashFunction::standard(0xF, HASHIDX_0_MATCH00_MASKxF, sizeof(HASHIDX_0_MATCH00_MASKxF) - 1, true)));
    REQUIRE(0 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_A, sizeof(HASHIDX_0_A) - 1, false)));
    REQUIRE(HASHIDX_0_A_REHASH_IDX == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_A, sizeof(HASHIDX_0_A) - 1, true)));
    REQUIRE(0 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_B, sizeof(HASHIDX_0_B) - 1, false)));
    REQUIRE(HASHIDX_0_B_REHASH_IDX == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_B, sizeof(HASHIDX_0_B) - 1, true)));
    REQUIRE(0 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_C, sizeof(HASHIDX_0_C) - 1, false)));
    REQUIRE(HASHIDX_0_C_REHASH_IDX == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_0_C, sizeof(HASHIDX_0_C) - 1, true)));
    REQUIRE(1 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_1_A, sizeof(HASHIDX_1_A) - 1, false)));
    REQUIRE(1 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_1_B, sizeof(HASHIDX_1_B) - 1, false)));
    REQUIRE(1 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_1_C, sizeof(HASHIDX_1_C) - 1, false)));
    REQUIRE(2 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_2_A, sizeof(HASHIDX_2_A) - 1, false)));
    REQUIRE(2 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_2_B, sizeof(HASHIDX_2_B) - 1, false)));
    REQUIRE(2 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_2_C, sizeof(HASHIDX_2_C) - 1, false)));
    REQUIRE(3 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_3_A, sizeof(HASHIDX_3_A) - 1, false)));
    REQUIRE(3 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_3_B, sizeof(HASHIDX_3_B) - 1, false)));
    REQUIRE(3 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_3_C, sizeof(HASHIDX_3_C) - 1, false)));
    REQUIRE(4 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_4_A, sizeof(HASHIDX_4_A) - 1, false)));
    REQUIRE(4 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_4_B, sizeof(HASHIDX_4_B) - 1, false)));
    REQUIRE(4 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_4_C, sizeof(HASHIDX_4_C) - 1, false)));
    REQUIRE(5 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_5_A, sizeof(HASHIDX_5_A) - 1, false)));
    REQUIRE(5 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_5_B, sizeof(HASHIDX_5_B) - 1, false)));
    REQUIRE(5 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_5_C, sizeof(HASHIDX_5_C) - 1, false)));
    REQUIRE(6 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_6_A, sizeof(HASHIDX_6_A) - 1, false)));
    REQUIRE(6 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_6_B, sizeof(HASHIDX_6_B) - 1, false)));
    REQUIRE(6 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_6_C, sizeof(HASHIDX_6_C) - 1, false)));
    REQUIRE(7 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_7_A, sizeof(HASHIDX_7_A) - 1, false)));
    REQUIRE(7 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_7_B, sizeof(HASHIDX_7_B) - 1, false)));
    REQUIRE(7 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_7_C, sizeof(HASHIDX_7_C) - 1, false)));
    REQUIRE(8 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_8_A, sizeof(HASHIDX_8_A) - 1, false)));
    REQUIRE(8 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_8_B, sizeof(HASHIDX_8_B) - 1, false)));
    REQUIRE(8 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_8_C, sizeof(HASHIDX_8_C) - 1, false)));
    REQUIRE(9 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_9_A, sizeof(HASHIDX_9_A) - 1, false)));
    REQUIRE(9 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_9_B, sizeof(HASHIDX_9_B) - 1, false)));
    REQUIRE(9 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_9_C, sizeof(HASHIDX_9_C) - 1, false)));
    REQUIRE(10 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_10_A, sizeof(HASHIDX_10_A) - 1, false)));
    REQUIRE(10 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_10_B, sizeof(HASHIDX_10_B) - 1, false)));
    REQUIRE(10 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_10_C, sizeof(HASHIDX_10_C) - 1, false)));
    REQUIRE(11 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_11_A, sizeof(HASHIDX_11_A) - 1, false)));
    REQUIRE(11 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_11_B, sizeof(HASHIDX_11_B) - 1, false)));
    REQUIRE(11 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_11_C, sizeof(HASHIDX_11_C) - 1, false)));
    REQUIRE(12 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_12_A, sizeof(HASHIDX_12_A) - 1, false)));
    REQUIRE(12 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_12_B, sizeof(HASHIDX_12_B) - 1, false)));
    REQUIRE(12 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_12_C, sizeof(HASHIDX_12_C) - 1, false)));
    REQUIRE(13 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_13_A, sizeof(HASHIDX_13_A) - 1, false)));
    REQUIRE(13 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_13_B, sizeof(HASHIDX_13_B) - 1, false)));
    REQUIRE(13 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_13_C, sizeof(HASHIDX_13_C) - 1, false)));
    REQUIRE(14 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_14_A, sizeof(HASHIDX_14_A) - 1, false)));
    REQUIRE(14 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_14_B, sizeof(HASHIDX_14_B) - 1, false)));
    REQUIRE(14 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_14_C, sizeof(HASHIDX_14_C) - 1, false)));
    REQUIRE(15 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_15_A, sizeof(HASHIDX_15_A) - 1, false)));
    REQUIRE(15 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_15_B, sizeof(HASHIDX_15_B) - 1, false)));
    REQUIRE(15 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_15_C, sizeof(HASHIDX_15_C) - 1, false)));
    REQUIRE(16 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_16_A, sizeof(HASHIDX_16_A) - 1, false)));
    REQUIRE(16 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_16_B, sizeof(HASHIDX_16_B) - 1, false)));
    REQUIRE(16 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_16_C, sizeof(HASHIDX_16_C) - 1, false)));
    REQUIRE(17 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_17_A, sizeof(HASHIDX_17_A) - 1, false)));
    REQUIRE(17 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_17_B, sizeof(HASHIDX_17_B) - 1, false)));
    REQUIRE(17 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_17_C, sizeof(HASHIDX_17_C) - 1, false)));
    REQUIRE(18 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_18_A, sizeof(HASHIDX_18_A) - 1, false)));
    REQUIRE(18 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_18_B, sizeof(HASHIDX_18_B) - 1, false)));
    REQUIRE(18 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_18_C, sizeof(HASHIDX_18_C) - 1, false)));
    REQUIRE(19 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_19_A, sizeof(HASHIDX_19_A) - 1, false)));
    REQUIRE(19 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_19_B, sizeof(HASHIDX_19_B) - 1, false)));
    REQUIRE(19 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_19_C, sizeof(HASHIDX_19_C) - 1, false)));
    REQUIRE(20 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_20_A, sizeof(HASHIDX_20_A) - 1, false)));
    REQUIRE(20 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_20_B, sizeof(HASHIDX_20_B) - 1, false)));
    REQUIRE(20 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_20_C, sizeof(HASHIDX_20_C) - 1, false)));
    REQUIRE(21 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_21_A, sizeof(HASHIDX_21_A) - 1, false)));
    REQUIRE(21 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_21_B, sizeof(HASHIDX_21_B) - 1, false)));
    REQUIRE(21 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_21_C, sizeof(HASHIDX_21_C) - 1, false)));
    REQUIRE(22 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_22_A, sizeof(HASHIDX_22_A) - 1, false)));
    REQUIRE(22 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_22_B, sizeof(HASHIDX_22_B) - 1, false)));
    REQUIRE(22 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_22_C, sizeof(HASHIDX_22_C) - 1, false)));
    REQUIRE(23 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_23_A, sizeof(HASHIDX_23_A) - 1, false)));
    REQUIRE(23 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_23_B, sizeof(HASHIDX_23_B) - 1, false)));
    REQUIRE(23 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_23_C, sizeof(HASHIDX_23_C) - 1, false)));
    REQUIRE(24 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_24_A, sizeof(HASHIDX_24_A) - 1, false)));
    REQUIRE(24 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_24_B, sizeof(HASHIDX_24_B) - 1, false)));
    REQUIRE(24 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_24_C, sizeof(HASHIDX_24_C) - 1, false)));
    REQUIRE(25 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_25_A, sizeof(HASHIDX_25_A) - 1, false)));
    REQUIRE(25 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_25_B, sizeof(HASHIDX_25_B) - 1, false)));
    REQUIRE(25 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_25_C, sizeof(HASHIDX_25_C) - 1, false)));
    REQUIRE(26 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_26_A, sizeof(HASHIDX_26_A) - 1, false)));
    REQUIRE(26 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_26_B, sizeof(HASHIDX_26_B) - 1, false)));
    REQUIRE(26 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_26_C, sizeof(HASHIDX_26_C) - 1, false)));
    REQUIRE(27 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_27_A, sizeof(HASHIDX_27_A) - 1, false)));
    REQUIRE(27 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_27_B, sizeof(HASHIDX_27_B) - 1, false)));
    REQUIRE(27 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_27_C, sizeof(HASHIDX_27_C) - 1, false)));
    REQUIRE(28 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_28_A, sizeof(HASHIDX_28_A) - 1, false)));
    REQUIRE(28 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_28_B, sizeof(HASHIDX_28_B) - 1, false)));
    REQUIRE(28 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_28_C, sizeof(HASHIDX_28_C) - 1, false)));
    REQUIRE(29 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_29_A, sizeof(HASHIDX_29_A) - 1, false)));
    REQUIRE(29 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_29_B, sizeof(HASHIDX_29_B) - 1, false)));
    REQUIRE(29 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_29_C, sizeof(HASHIDX_29_C) - 1, false)));
    REQUIRE(30 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_30_A, sizeof(HASHIDX_30_A) - 1, false)));
    REQUIRE(30 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_30_B, sizeof(HASHIDX_30_B) - 1, false)));
    REQUIRE(30 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_30_C, sizeof(HASHIDX_30_C) - 1, false)));
    REQUIRE(31 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_31_A, sizeof(HASHIDX_31_A) - 1, false)));
    REQUIRE(31 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_31_B, sizeof(HASHIDX_31_B) - 1, false)));
    REQUIRE(31 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_31_C, sizeof(HASHIDX_31_C) - 1, false)));
    REQUIRE(32 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_32_A, sizeof(HASHIDX_32_A) - 1, false)));
    REQUIRE(32 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_32_B, sizeof(HASHIDX_32_B) - 1, false)));
    REQUIRE(32 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_32_C, sizeof(HASHIDX_32_C) - 1, false)));
    REQUIRE(33 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_33_A, sizeof(HASHIDX_33_A) - 1, false)));
    REQUIRE(33 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_33_B, sizeof(HASHIDX_33_B) - 1, false)));
    REQUIRE(33 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_33_C, sizeof(HASHIDX_33_C) - 1, false)));
    REQUIRE(34 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_34_A, sizeof(HASHIDX_34_A) - 1, false)));
    REQUIRE(34 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_34_B, sizeof(HASHIDX_34_B) - 1, false)));
    REQUIRE(34 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_34_C, sizeof(HASHIDX_34_C) - 1, false)));
    REQUIRE(35 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_35_A, sizeof(HASHIDX_35_A) - 1, false)));
    REQUIRE(35 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_35_B, sizeof(HASHIDX_35_B) - 1, false)));
    REQUIRE(35 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_35_C, sizeof(HASHIDX_35_C) - 1, false)));
    REQUIRE(36 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_36_A, sizeof(HASHIDX_36_A) - 1, false)));
    REQUIRE(36 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_36_B, sizeof(HASHIDX_36_B) - 1, false)));
    REQUIRE(36 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_36_C, sizeof(HASHIDX_36_C) - 1, false)));
    REQUIRE(37 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_37_A, sizeof(HASHIDX_37_A) - 1, false)));
    REQUIRE(37 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_37_B, sizeof(HASHIDX_37_B) - 1, false)));
    REQUIRE(37 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_37_C, sizeof(HASHIDX_37_C) - 1, false)));
    REQUIRE(38 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_38_A, sizeof(HASHIDX_38_A) - 1, false)));
    REQUIRE(38 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_38_B, sizeof(HASHIDX_38_B) - 1, false)));
    REQUIRE(38 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_38_C, sizeof(HASHIDX_38_C) - 1, false)));
    REQUIRE(39 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_39_A, sizeof(HASHIDX_39_A) - 1, false)));
    REQUIRE(39 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_39_B, sizeof(HASHIDX_39_B) - 1, false)));
    REQUIRE(39 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_39_C, sizeof(HASHIDX_39_C) - 1, false)));
    REQUIRE(40 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_40_A, sizeof(HASHIDX_40_A) - 1, false)));
    REQUIRE(40 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_40_B, sizeof(HASHIDX_40_B) - 1, false)));
    REQUIRE(40 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_40_C, sizeof(HASHIDX_40_C) - 1, false)));
    REQUIRE(41 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_41_A, sizeof(HASHIDX_41_A) - 1, false)));
    REQUIRE(41 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_41_B, sizeof(HASHIDX_41_B) - 1, false)));
    REQUIRE(41 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_41_C, sizeof(HASHIDX_41_C) - 1, false)));
    REQUIRE(42 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_42_A, sizeof(HASHIDX_42_A) - 1, false)));
    REQUIRE(42 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_42_B, sizeof(HASHIDX_42_B) - 1, false)));
    REQUIRE(42 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_42_C, sizeof(HASHIDX_42_C) - 1, false)));
    REQUIRE(43 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_43_A, sizeof(HASHIDX_43_A) - 1, false)));
    REQUIRE(43 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_43_B, sizeof(HASHIDX_43_B) - 1, false)));
    REQUIRE(43 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_43_C, sizeof(HASHIDX_43_C) - 1, false)));
    REQUIRE(44 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_44_A, sizeof(HASHIDX_44_A) - 1, false)));
    REQUIRE(44 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_44_B, sizeof(HASHIDX_44_B) - 1, false)));
    REQUIRE(44 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_44_C, sizeof(HASHIDX_44_C) - 1, false)));
    REQUIRE(45 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_45_A, sizeof(HASHIDX_45_A) - 1, false)));
    REQUIRE(45 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_45_B, sizeof(HASHIDX_45_B) - 1, false)));
    REQUIRE(45 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_45_C, sizeof(HASHIDX_45_C) - 1, false)));
    REQUIRE(46 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_46_A, sizeof(HASHIDX_46_A) - 1, false)));
    REQUIRE(46 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_46_B, sizeof(HASHIDX_46_B) - 1, false)));
    REQUIRE(46 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_46_C, sizeof(HASHIDX_46_C) - 1, false)));
    REQUIRE(47 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_47_A, sizeof(HASHIDX_47_A) - 1, false)));
    REQUIRE(47 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_47_B, sizeof(HASHIDX_47_B) - 1, false)));
    REQUIRE(47 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_47_C, sizeof(HASHIDX_47_C) - 1, false)));
    REQUIRE(48 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_48_A, sizeof(HASHIDX_48_A) - 1, false)));
    REQUIRE(48 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_48_B, sizeof(HASHIDX_48_B) - 1, false)));
    REQUIRE(48 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_48_C, sizeof(HASHIDX_48_C) - 1, false)));
    REQUIRE(49 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_49_A, sizeof(HASHIDX_49_A) - 1, false)));
    REQUIRE(49 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_49_B, sizeof(HASHIDX_49_B) - 1, false)));
    REQUIRE(49 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_49_C, sizeof(HASHIDX_49_C) - 1, false)));
    REQUIRE(50 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_50_A, sizeof(HASHIDX_50_A) - 1, false)));
    REQUIRE(50 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_50_B, sizeof(HASHIDX_50_B) - 1, false)));
    REQUIRE(50 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_50_C, sizeof(HASHIDX_50_C) - 1, false)));
    REQUIRE(51 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_51_A, sizeof(HASHIDX_51_A) - 1, false)));
    REQUIRE(51 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_51_B, sizeof(HASHIDX_51_B) - 1, false)));
    REQUIRE(51 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_51_C, sizeof(HASHIDX_51_C) - 1, false)));
    REQUIRE(52 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_52_A, sizeof(HASHIDX_52_A) - 1, false)));
    REQUIRE(52 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_52_B, sizeof(HASHIDX_52_B) - 1, false)));
    REQUIRE(52 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_52_C, sizeof(HASHIDX_52_C) - 1, false)));
    REQUIRE(53 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_53_A, sizeof(HASHIDX_53_A) - 1, false)));
    REQUIRE(53 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_53_B, sizeof(HASHIDX_53_B) - 1, false)));
    REQUIRE(53 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_53_C, sizeof(HASHIDX_53_C) - 1, false)));
    REQUIRE(54 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_54_A, sizeof(HASHIDX_54_A) - 1, false)));
    REQUIRE(54 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_54_B, sizeof(HASHIDX_54_B) - 1, false)));
    REQUIRE(54 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_54_C, sizeof(HASHIDX_54_C) - 1, false)));
    REQUIRE(55 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_55_A, sizeof(HASHIDX_55_A) - 1, false)));
    REQUIRE(55 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_55_B, sizeof(HASHIDX_55_B) - 1, false)));
    REQUIRE(55 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_55_C, sizeof(HASHIDX_55_C) - 1, false)));
    REQUIRE(56 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_56_A, sizeof(HASHIDX_56_A) - 1, false)));
    REQUIRE(56 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_56_B, sizeof(HASHIDX_56_B) - 1, false)));
    REQUIRE(56 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_56_C, sizeof(HASHIDX_56_C) - 1, false)));
    REQUIRE(57 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_57_A, sizeof(HASHIDX_57_A) - 1, false)));
    REQUIRE(57 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_57_B, sizeof(HASHIDX_57_B) - 1, false)));
    REQUIRE(57 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_57_C, sizeof(HASHIDX_57_C) - 1, false)));
    REQUIRE(58 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_58_A, sizeof(HASHIDX_58_A) - 1, false)));
    REQUIRE(58 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_58_B, sizeof(HASHIDX_58_B) - 1, false)));
    REQUIRE(58 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_58_C, sizeof(HASHIDX_58_C) - 1, false)));
    REQUIRE(59 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_59_A, sizeof(HASHIDX_59_A) - 1, false)));
    REQUIRE(59 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_59_B, sizeof(HASHIDX_59_B) - 1, false)));
    REQUIRE(59 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_59_C, sizeof(HASHIDX_59_C) - 1, false)));
    REQUIRE(60 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_60_A, sizeof(HASHIDX_60_A) - 1, false)));
    REQUIRE(60 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_60_B, sizeof(HASHIDX_60_B) - 1, false)));
    REQUIRE(60 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_60_C, sizeof(HASHIDX_60_C) - 1, false)));
    REQUIRE(61 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_61_A, sizeof(HASHIDX_61_A) - 1, false)));
    REQUIRE(61 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_61_B, sizeof(HASHIDX_61_B) - 1, false)));
    REQUIRE(61 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_61_C, sizeof(HASHIDX_61_C) - 1, false)));
    REQUIRE(62 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_62_A, sizeof(HASHIDX_62_A) - 1, false)));
    REQUIRE(62 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_62_B, sizeof(HASHIDX_62_B) - 1, false)));
    REQUIRE(62 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_62_C, sizeof(HASHIDX_62_C) - 1, false)));
    REQUIRE(63 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_63_A, sizeof(HASHIDX_63_A) - 1, false)));
    REQUIRE(63 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_63_B, sizeof(HASHIDX_63_B) - 1, false)));
    REQUIRE(63 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_63_C, sizeof(HASHIDX_63_C) - 1, false)));
    REQUIRE(64 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_64_A, sizeof(HASHIDX_64_A) - 1, false)));
    REQUIRE(64 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_64_B, sizeof(HASHIDX_64_B) - 1, false)));
    REQUIRE(64 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_64_C, sizeof(HASHIDX_64_C) - 1, false)));
    REQUIRE(65 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_65_A, sizeof(HASHIDX_65_A) - 1, false)));
    REQUIRE(65 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_65_B, sizeof(HASHIDX_65_B) - 1, false)));
    REQUIRE(65 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_65_C, sizeof(HASHIDX_65_C) - 1, false)));
    REQUIRE(66 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_66_A, sizeof(HASHIDX_66_A) - 1, false)));
    REQUIRE(66 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_66_B, sizeof(HASHIDX_66_B) - 1, false)));
    REQUIRE(66 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_66_C, sizeof(HASHIDX_66_C) - 1, false)));
    REQUIRE(67 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_67_A, sizeof(HASHIDX_67_A) - 1, false)));
    REQUIRE(67 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_67_B, sizeof(HASHIDX_67_B) - 1, false)));
    REQUIRE(67 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_67_C, sizeof(HASHIDX_67_C) - 1, false)));
    REQUIRE(68 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_68_A, sizeof(HASHIDX_68_A) - 1, false)));
    REQUIRE(68 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_68_B, sizeof(HASHIDX_68_B) - 1, false)));
    REQUIRE(68 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_68_C, sizeof(HASHIDX_68_C) - 1, false)));
    REQUIRE(69 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_69_A, sizeof(HASHIDX_69_A) - 1, false)));
    REQUIRE(69 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_69_B, sizeof(HASHIDX_69_B) - 1, false)));
    REQUIRE(69 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_69_C, sizeof(HASHIDX_69_C) - 1, false)));
    REQUIRE(70 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_70_A, sizeof(HASHIDX_70_A) - 1, false)));
    REQUIRE(70 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_70_B, sizeof(HASHIDX_70_B) - 1, false)));
    REQUIRE(70 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_70_C, sizeof(HASHIDX_70_C) - 1, false)));
    REQUIRE(71 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_71_A, sizeof(HASHIDX_71_A) - 1, false)));
    REQUIRE(71 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_71_B, sizeof(HASHIDX_71_B) - 1, false)));
    REQUIRE(71 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_71_C, sizeof(HASHIDX_71_C) - 1, false)));
    REQUIRE(72 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_72_A, sizeof(HASHIDX_72_A) - 1, false)));
    REQUIRE(72 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_72_B, sizeof(HASHIDX_72_B) - 1, false)));
    REQUIRE(72 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_72_C, sizeof(HASHIDX_72_C) - 1, false)));
    REQUIRE(73 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_73_A, sizeof(HASHIDX_73_A) - 1, false)));
    REQUIRE(73 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_73_B, sizeof(HASHIDX_73_B) - 1, false)));
    REQUIRE(73 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_73_C, sizeof(HASHIDX_73_C) - 1, false)));
    REQUIRE(74 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_74_A, sizeof(HASHIDX_74_A) - 1, false)));
    REQUIRE(74 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_74_B, sizeof(HASHIDX_74_B) - 1, false)));
    REQUIRE(74 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_74_C, sizeof(HASHIDX_74_C) - 1, false)));
    REQUIRE(75 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_75_A, sizeof(HASHIDX_75_A) - 1, false)));
    REQUIRE(75 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_75_B, sizeof(HASHIDX_75_B) - 1, false)));
    REQUIRE(75 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_75_C, sizeof(HASHIDX_75_C) - 1, false)));
    REQUIRE(76 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_76_A, sizeof(HASHIDX_76_A) - 1, false)));
    REQUIRE(76 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_76_B, sizeof(HASHIDX_76_B) - 1, false)));
    REQUIRE(76 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_76_C, sizeof(HASHIDX_76_C) - 1, false)));
    REQUIRE(77 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_77_A, sizeof(HASHIDX_77_A) - 1, false)));
    REQUIRE(77 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_77_B, sizeof(HASHIDX_77_B) - 1, false)));
    REQUIRE(77 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_77_C, sizeof(HASHIDX_77_C) - 1, false)));
    REQUIRE(78 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_78_A, sizeof(HASHIDX_78_A) - 1, false)));
    REQUIRE(78 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_78_B, sizeof(HASHIDX_78_B) - 1, false)));
    REQUIRE(78 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_78_C, sizeof(HASHIDX_78_C) - 1, false)));
    REQUIRE(79 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_79_A, sizeof(HASHIDX_79_A) - 1, false)));
    REQUIRE(79 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_79_B, sizeof(HASHIDX_79_B) - 1, false)));
    REQUIRE(79 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_79_C, sizeof(HASHIDX_79_C) - 1, false)));
    REQUIRE(80 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_80_A, sizeof(HASHIDX_80_A) - 1, false)));
    REQUIRE(80 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_80_B, sizeof(HASHIDX_80_B) - 1, false)));
    REQUIRE(80 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_80_C, sizeof(HASHIDX_80_C) - 1, false)));
    REQUIRE(81 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_81_A, sizeof(HASHIDX_81_A) - 1, false)));
    REQUIRE(81 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_81_B, sizeof(HASHIDX_81_B) - 1, false)));
    REQUIRE(81 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_81_C, sizeof(HASHIDX_81_C) - 1, false)));
    REQUIRE(82 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_82_A, sizeof(HASHIDX_82_A) - 1, false)));
    REQUIRE(82 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_82_B, sizeof(HASHIDX_82_B) - 1, false)));
    REQUIRE(82 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_82_C, sizeof(HASHIDX_82_C) - 1, false)));
    REQUIRE(83 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_83_A, sizeof(HASHIDX_83_A) - 1, false)));
    REQUIRE(83 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_83_B, sizeof(HASHIDX_83_B) - 1, false)));
    REQUIRE(83 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_83_C, sizeof(HASHIDX_83_C) - 1, false)));
    REQUIRE(84 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_84_A, sizeof(HASHIDX_84_A) - 1, false)));
    REQUIRE(84 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_84_B, sizeof(HASHIDX_84_B) - 1, false)));
    REQUIRE(84 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_84_C, sizeof(HASHIDX_84_C) - 1, false)));
    REQUIRE(85 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_85_A, sizeof(HASHIDX_85_A) - 1, false)));
    REQUIRE(85 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_85_B, sizeof(HASHIDX_85_B) - 1, false)));
    REQUIRE(85 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_85_C, sizeof(HASHIDX_85_C) - 1, false)));
    REQUIRE(86 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_86_A, sizeof(HASHIDX_86_A) - 1, false)));
    REQUIRE(86 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_86_B, sizeof(HASHIDX_86_B) - 1, false)));
    REQUIRE(86 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_86_C, sizeof(HASHIDX_86_C) - 1, false)));
    REQUIRE(87 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_87_A, sizeof(HASHIDX_87_A) - 1, false)));
    REQUIRE(87 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_87_B, sizeof(HASHIDX_87_B) - 1, false)));
    REQUIRE(87 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_87_C, sizeof(HASHIDX_87_C) - 1, false)));
    REQUIRE(88 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_88_A, sizeof(HASHIDX_88_A) - 1, false)));
    REQUIRE(88 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_88_B, sizeof(HASHIDX_88_B) - 1, false)));
    REQUIRE(88 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_88_C, sizeof(HASHIDX_88_C) - 1, false)));
    REQUIRE(89 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_89_A, sizeof(HASHIDX_89_A) - 1, false)));
    REQUIRE(89 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_89_B, sizeof(HASHIDX_89_B) - 1, false)));
    REQUIRE(89 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_89_C, sizeof(HASHIDX_89_C) - 1, false)));
    REQUIRE(90 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_90_A, sizeof(HASHIDX_90_A) - 1, false)));
    REQUIRE(90 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_90_B, sizeof(HASHIDX_90_B) - 1, false)));
    REQUIRE(90 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_90_C, sizeof(HASHIDX_90_C) - 1, false)));
    REQUIRE(91 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_91_A, sizeof(HASHIDX_91_A) - 1, false)));
    REQUIRE(91 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_91_B, sizeof(HASHIDX_91_B) - 1, false)));
    REQUIRE(91 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_91_C, sizeof(HASHIDX_91_C) - 1, false)));
    REQUIRE(92 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_92_A, sizeof(HASHIDX_92_A) - 1, false)));
    REQUIRE(92 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_92_B, sizeof(HASHIDX_92_B) - 1, false)));
    REQUIRE(92 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_92_C, sizeof(HASHIDX_92_C) - 1, false)));
    REQUIRE(93 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_93_A, sizeof(HASHIDX_93_A) - 1, false)));
    REQUIRE(93 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_93_B, sizeof(HASHIDX_93_B) - 1, false)));
    REQUIRE(93 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_93_C, sizeof(HASHIDX_93_C) - 1, false)));
    REQUIRE(94 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_94_A, sizeof(HASHIDX_94_A) - 1, false)));
    REQUIRE(94 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_94_B, sizeof(HASHIDX_94_B) - 1, false)));
    REQUIRE(94 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_94_C, sizeof(HASHIDX_94_C) - 1, false)));
    REQUIRE(95 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_95_A, sizeof(HASHIDX_95_A) - 1, false)));
    REQUIRE(95 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_95_B, sizeof(HASHIDX_95_B) - 1, false)));
    REQUIRE(95 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_95_C, sizeof(HASHIDX_95_C) - 1, false)));
    REQUIRE(96 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_96_A, sizeof(HASHIDX_96_A) - 1, false)));
    REQUIRE(96 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_96_B, sizeof(HASHIDX_96_B) - 1, false)));
    REQUIRE(96 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_96_C, sizeof(HASHIDX_96_C) - 1, false)));
    REQUIRE(97 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_97_A, sizeof(HASHIDX_97_A) - 1, false)));
    REQUIRE(97 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_97_B, sizeof(HASHIDX_97_B) - 1, false)));
    REQUIRE(97 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_97_C, sizeof(HASHIDX_97_C) - 1, false)));
    REQUIRE(98 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_98_A, sizeof(HASHIDX_98_A) - 1, false)));
    REQUIRE(98 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_98_B, sizeof(HASHIDX_98_B) - 1, false)));
    REQUIRE(98 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_98_C, sizeof(HASHIDX_98_C) - 1, false)));
    REQUIRE(99 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_99_A, sizeof(HASHIDX_99_A) - 1, false)));
    REQUIRE(99 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_99_B, sizeof(HASHIDX_99_B) - 1, false)));
    REQUIRE(99 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_99_C, sizeof(HASHIDX_99_C) - 1, false)));
    REQUIRE(100 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_100_A, sizeof(HASHIDX_100_A) - 1, false)));
    REQUIRE(100 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_100_B, sizeof(HASHIDX_100_B) - 1, false)));
    REQUIRE(100 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_100_C, sizeof(HASHIDX_100_C) - 1, false)));
    REQUIRE(101 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_101_A, sizeof(HASHIDX_101_A) - 1, false)));
    REQUIRE(101 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_101_B, sizeof(HASHIDX_101_B) - 1, false)));
    REQUIRE(101 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_101_C, sizeof(HASHIDX_101_C) - 1, false)));
    REQUIRE(102 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_102_A, sizeof(HASHIDX_102_A) - 1, false)));
    REQUIRE(102 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_102_B, sizeof(HASHIDX_102_B) - 1, false)));
    REQUIRE(102 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_102_C, sizeof(HASHIDX_102_C) - 1, false)));
    REQUIRE(103 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_103_A, sizeof(HASHIDX_103_A) - 1, false)));
    REQUIRE(103 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_103_B, sizeof(HASHIDX_103_B) - 1, false)));
    REQUIRE(103 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_103_C, sizeof(HASHIDX_103_C) - 1, false)));
    REQUIRE(104 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_104_A, sizeof(HASHIDX_104_A) - 1, false)));
    REQUIRE(104 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_104_B, sizeof(HASHIDX_104_B) - 1, false)));
    REQUIRE(104 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_104_C, sizeof(HASHIDX_104_C) - 1, false)));
    REQUIRE(105 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_105_A, sizeof(HASHIDX_105_A) - 1, false)));
    REQUIRE(105 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_105_B, sizeof(HASHIDX_105_B) - 1, false)));
    REQUIRE(105 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_105_C, sizeof(HASHIDX_105_C) - 1, false)));
    REQUIRE(106 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_106_A, sizeof(HASHIDX_106_A) - 1, false)));
    REQUIRE(106 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_106_B, sizeof(HASHIDX_106_B) - 1, false)));
    REQUIRE(106 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_106_C, sizeof(HASHIDX_106_C) - 1, false)));
    REQUIRE(107 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_107_A, sizeof(HASHIDX_107_A) - 1, false)));
    REQUIRE(107 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_107_B, sizeof(HASHIDX_107_B) - 1, false)));
    REQUIRE(107 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_107_C, sizeof(HASHIDX_107_C) - 1, false)));
    REQUIRE(108 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_108_A, sizeof(HASHIDX_108_A) - 1, false)));
    REQUIRE(108 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_108_B, sizeof(HASHIDX_108_B) - 1, false)));
    REQUIRE(108 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_108_C, sizeof(HASHIDX_108_C) - 1, false)));
    REQUIRE(109 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_109_A, sizeof(HASHIDX_109_A) - 1, false)));
    REQUIRE(109 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_109_B, sizeof(HASHIDX_109_B) - 1, false)));
    REQUIRE(109 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_109_C, sizeof(HASHIDX_109_C) - 1, false)));
    REQUIRE(110 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_110_A, sizeof(HASHIDX_110_A) - 1, false)));
    REQUIRE(110 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_110_B, sizeof(HASHIDX_110_B) - 1, false)));
    REQUIRE(110 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_110_C, sizeof(HASHIDX_110_C) - 1, false)));
    REQUIRE(111 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_111_A, sizeof(HASHIDX_111_A) - 1, false)));
    REQUIRE(111 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_111_B, sizeof(HASHIDX_111_B) - 1, false)));
    REQUIRE(111 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_111_C, sizeof(HASHIDX_111_C) - 1, false)));
    REQUIRE(112 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_112_A, sizeof(HASHIDX_112_A) - 1, false)));
    REQUIRE(112 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_112_B, sizeof(HASHIDX_112_B) - 1, false)));
    REQUIRE(112 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_112_C, sizeof(HASHIDX_112_C) - 1, false)));
    REQUIRE(113 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_113_A, sizeof(HASHIDX_113_A) - 1, false)));
    REQUIRE(113 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_113_B, sizeof(HASHIDX_113_B) - 1, false)));
    REQUIRE(113 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_113_C, sizeof(HASHIDX_113_C) - 1, false)));
    REQUIRE(114 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_114_A, sizeof(HASHIDX_114_A) - 1, false)));
    REQUIRE(114 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_114_B, sizeof(HASHIDX_114_B) - 1, false)));
    REQUIRE(114 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_114_C, sizeof(HASHIDX_114_C) - 1, false)));
    REQUIRE(115 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_115_A, sizeof(HASHIDX_115_A) - 1, false)));
    REQUIRE(115 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_115_B, sizeof(HASHIDX_115_B) - 1, false)));
    REQUIRE(115 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_115_C, sizeof(HASHIDX_115_C) - 1, false)));
    REQUIRE(116 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_116_A, sizeof(HASHIDX_116_A) - 1, false)));
    REQUIRE(116 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_116_B, sizeof(HASHIDX_116_B) - 1, false)));
    REQUIRE(116 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_116_C, sizeof(HASHIDX_116_C) - 1, false)));
    REQUIRE(117 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_117_A, sizeof(HASHIDX_117_A) - 1, false)));
    REQUIRE(117 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_117_B, sizeof(HASHIDX_117_B) - 1, false)));
    REQUIRE(117 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_117_C, sizeof(HASHIDX_117_C) - 1, false)));
    REQUIRE(118 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_118_A, sizeof(HASHIDX_118_A) - 1, false)));
    REQUIRE(118 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_118_B, sizeof(HASHIDX_118_B) - 1, false)));
    REQUIRE(118 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_118_C, sizeof(HASHIDX_118_C) - 1, false)));
    REQUIRE(119 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_119_A, sizeof(HASHIDX_119_A) - 1, false)));
    REQUIRE(119 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_119_B, sizeof(HASHIDX_119_B) - 1, false)));
    REQUIRE(119 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_119_C, sizeof(HASHIDX_119_C) - 1, false)));
    REQUIRE(120 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_120_A, sizeof(HASHIDX_120_A) - 1, false)));
    REQUIRE(120 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_120_B, sizeof(HASHIDX_120_B) - 1, false)));
    REQUIRE(120 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_120_C, sizeof(HASHIDX_120_C) - 1, false)));
    REQUIRE(121 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_121_A, sizeof(HASHIDX_121_A) - 1, false)));
    REQUIRE(121 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_121_B, sizeof(HASHIDX_121_B) - 1, false)));
    REQUIRE(121 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_121_C, sizeof(HASHIDX_121_C) - 1, false)));
    REQUIRE(122 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_122_A, sizeof(HASHIDX_122_A) - 1, false)));
    REQUIRE(122 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_122_B, sizeof(HASHIDX_122_B) - 1, false)));
    REQUIRE(122 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_122_C, sizeof(HASHIDX_122_C) - 1, false)));
    REQUIRE(123 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_123_A, sizeof(HASHIDX_123_A) - 1, false)));
    REQUIRE(123 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_123_B, sizeof(HASHIDX_123_B) - 1, false)));
    REQUIRE(123 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_123_C, sizeof(HASHIDX_123_C) - 1, false)));
    REQUIRE(124 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_124_A, sizeof(HASHIDX_124_A) - 1, false)));
    REQUIRE(124 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_124_B, sizeof(HASHIDX_124_B) - 1, false)));
    REQUIRE(124 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_124_C, sizeof(HASHIDX_124_C) - 1, false)));
    REQUIRE(125 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_125_A, sizeof(HASHIDX_125_A) - 1, false)));
    REQUIRE(125 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_125_B, sizeof(HASHIDX_125_B) - 1, false)));
    REQUIRE(125 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_125_C, sizeof(HASHIDX_125_C) - 1, false)));
    REQUIRE(126 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_126_A, sizeof(HASHIDX_126_A) - 1, false)));
    REQUIRE(126 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_126_B, sizeof(HASHIDX_126_B) - 1, false)));
    REQUIRE(126 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_126_C, sizeof(HASHIDX_126_C) - 1, false)));
    REQUIRE(127 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_127_A, sizeof(HASHIDX_127_A) - 1, false)));
    REQUIRE(127 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_127_B, sizeof(HASHIDX_127_B) - 1, false)));
    REQUIRE(127 == (HashFunction::standard(AQLOG_TIER_0_MASK, HASHIDX_127_C, sizeof(HASHIDX_127_C) - 1, false)));
}



//=============================== End of File ==================================
