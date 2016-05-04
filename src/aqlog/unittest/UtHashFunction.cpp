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
#include "HashFunction.h"




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
TEST(given_HashFunction_when_HashMany_then_FindCollisions)
{
    uint32_t maxTierMask = AQLOG_TIER_0_MASK;
    uint32_t minTierMask = AQLOG_TIER_0_MASK;
    if (AQLOG_TIER_1_MASK > maxTierMask)
    {
        maxTierMask = AQLOG_TIER_1_MASK;
    }
    if (AQLOG_TIER_2_MASK > maxTierMask)
    {
        maxTierMask = AQLOG_TIER_2_MASK;
    }
    if (AQLOG_TIER_1_MASK < minTierMask)
    {
        minTierMask = AQLOG_TIER_1_MASK;
    }
    if (AQLOG_TIER_2_MASK < minTierMask)
    {
        minTierMask = AQLOG_TIER_2_MASK;
    }

    map<uint32_t, vector<string> > collisions;
    HashFunction_fn hashFn = HashFunction::djb2b;
    for (size_t i = 0; i < 1000; ++i)
    {
        std::string s;

        for (size_t j = 0; j < 7; ++j)
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
            s += ch;
        }
        uint32_t hash = hashFn(s.c_str(), s.size());
        hash &= maxTierMask;
        collisions[hash].push_back(s);
    }

    vector<uint32_t> printKeys;
    for (map<uint32_t, vector<string> >::iterator it = collisions.begin();
    it != collisions.end(); it++)
    {
        if (it->second.size() >= 3)
        {
            // Make sure we don't have a matching lower tier.
            size_t i;
            for (i = 0; i < printKeys.size(); ++i)
            {
                if ((printKeys[i] & minTierMask) == (it->first & minTierMask))
                {
                    break;
                }
            }
            if (i == printKeys.size())
            {
                printKeys.push_back(it->first);
            }
        }
    }

    // Generate only 6 samples.
    while (printKeys.size() > 6)
    {
        printKeys.erase(printKeys.begin() + (rand() % printKeys.size()));
    }
    REQUIRE(printKeys.size() == 6);

    for (size_t i = 0; i < printKeys.size(); ++i)
    {
        const vector<string>& v = collisions[printKeys[i]];

        cout << "// " << v.size() << " collisions" << endl;
        for (size_t j = 0; j < v.size(); ++j)
        {
            cout << "#define COLLTEST_" << (char)('A' + i) << "_" << (j + 1) << " \"" << v[j] << "\"" << endl;
        }
        cout << endl;
    }
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
        AQLOG_HASHTIER_INLINE(inlineHash, strings[i].c_str(), strings[i].size());

        uint32_t externHash = HashFunction::standard(strings[i].c_str(), strings[i].size());

        cout << "Hashing \"" << strings[i] << "\"" << endl;
        REQUIRE(inlineHash == externHash);
    }
}

//------------------------------------------------------------------------------
TEST(given_SlashInString_when_HashCalculated_then_OnlyPartAfterSlashConsidered)
{
#define STR1 "sdfasdf/w33234"
#define STR2 "546gd2f/w33234"
    REQUIRE(HashFunction::standard(STR1, sizeof(STR1) - 1) == HashFunction::standard(STR2, sizeof(STR2) - 1));
#undef STR1
#undef STR2
}

//------------------------------------------------------------------------------
TEST(given_BackslashInString_when_HashCalculated_then_OnlyPartAfterBackslashConsidered)
{
#define STR1 "sdfds1f\\633234"
#define STR2 "546gs2f\\633234"
    REQUIRE(HashFunction::standard(STR1, sizeof(STR1) - 1) == HashFunction::standard(STR2, sizeof(STR2) - 1));
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
    REQUIRE(HashFunction::standard(s1.str().c_str(), s1.str().size()) == HashFunction::standard(s2.str().c_str(), s2.str().size()));
}




//=============================== End of File ==================================
