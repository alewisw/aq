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

#include "HashFunction.h"

using namespace aqlog;
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

// Calculates the hash value of a string str_ consisting of strLen characters
// in the string.  The hash is returned.
static AQLOG_HASH_EXTERN_ATTRIBUTE uint32_t AQLog_HashTierExtern(const char *str, size_t strLen);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

const uint32_t *AQLog_LevelHashTable_g;




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
AQLOG_HASH_EXTERN_ATTRIBUTE bool AQLog_HashIsLevelExtern(int level, const char *str1, size_t str1Size, const char *str2, size_t str2Size, const char *str3, size_t str3Size)
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




//=============================== End of File ==================================
