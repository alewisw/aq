//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "HashFunction.h"

#include "AQLog.h"

using namespace std;

namespace aqlog
{




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
uint32_t HashFunction::standard(uint32_t mask, const char *str, size_t strLen, 
    bool zeroLengthHashToZero)
{
    if (strLen == 0 && zeroLengthHashToZero)
    {
        return 0;
    }
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = AQLOG_HASH_INIT;
    size_t i = strLen;
    while (i > limit)
    {
        i--;
        if (AQLOG_HASH_ISEND(str[i]))
        {
            break;
        }

        hash = AQLOG_HASH_STEP(hash, AQLOG_HASH_CHARMAP(str[i]));
    }
    hash &= mask;
    if (hash == 0 && zeroLengthHashToZero)
    {
        hash = str[strLen - 1] & mask;
        if (hash == 0)
        {
            hash = 1;
        }
    }
    return hash;
}

//------------------------------------------------------------------------------
uint32_t HashFunction::djb2(uint32_t mask, const char *str, size_t strLen,
    bool zeroLengthHashToZero)
{
    if (strLen == 0 && zeroLengthHashToZero)
    {
        return 0;
    }
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = AQLOG_HASH_INIT;
    size_t i = strLen;
    while (i > limit)
    {
        i--;
        if (AQLOG_HASH_ISEND(str[i]))
        {
            break;
        }

        hash = ((hash << 5) + hash) + str[strLen];
    }
    hash &= mask;
    if (hash == 0 && zeroLengthHashToZero)
    {
        hash = str[strLen - 1] & mask;
        if (hash == 0)
        {
            hash = 1;
        }
    }
    return hash;
}

//------------------------------------------------------------------------------
uint32_t HashFunction::djb2a(uint32_t mask, const char *str, size_t strLen,
    bool zeroLengthHashToZero)
{
    if (strLen == 0 && zeroLengthHashToZero)
    {
        return 0;
    }
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = AQLOG_HASH_INIT;
    size_t i = strLen;
    while (i > limit)
    {
        i--;
        if (AQLOG_HASH_ISEND(str[i]))
        {
            break;
        }

        hash = ((hash << 5) + hash) ^ str[strLen];
    }
    hash &= mask;
    if (hash == 0 && zeroLengthHashToZero)
    {
        hash = str[strLen - 1] & mask;
        if (hash == 0)
        {
            hash = 1;
        }
    }
    return hash;
}

//------------------------------------------------------------------------------
uint32_t HashFunction::sdbm(uint32_t mask, const char *str, size_t strLen,
    bool zeroLengthHashToZero)
{
    if (strLen == 0 && zeroLengthHashToZero)
    {
        return 0;
    }
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = AQLOG_HASH_INIT;
    size_t i = strLen;
    while (i > limit)
    {
        i--;
        if (AQLOG_HASH_ISEND(str[i]))
        {
            break;
        }

        hash = str[strLen] + (hash << 6) + (hash << 16) - hash;
    }
    hash &= mask;
    if (hash == 0 && zeroLengthHashToZero)
    {
        hash = str[strLen - 1] & mask;
        if (hash == 0)
        {
            hash = 1;
        }
    }
    return hash;
}




}
//=============================== End of File ==================================
