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
uint32_t HashFunction::standard(const char *str, size_t strLen)
{
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = AQLOG_HASH_INIT;

    while (strLen > limit)
    {
        strLen--;
        if (AQLOG_HASH_ISEND(str[strLen]))
        {
            return hash;
        }

        hash = AQLOG_HASH_STEP(hash, AQLOG_HASH_CHARMAP(str[strLen]));
    }
    return hash;
}

//------------------------------------------------------------------------------
uint32_t HashFunction::djb2a(const char *str, size_t strLen)
{
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = 5381;

    while (strLen > limit)
    {
        strLen--;
        if (AQLOG_HASH_ISEND(str[strLen]))
        {
            return hash;
        }

        hash = ((hash << 5) + hash) + str[strLen];
    }
    return hash;
}

//------------------------------------------------------------------------------
uint32_t HashFunction::djb2b(const char *str, size_t strLen)
{
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = 5381;

    while (strLen > limit)
    {
        strLen--;
        if (AQLOG_HASH_ISEND(str[strLen]))
        {
            return hash;
        }

        hash = ((hash << 5) + hash) ^ str[strLen];
    }
    return hash;
}

//------------------------------------------------------------------------------
uint32_t HashFunction::sdbm(const char *str, size_t strLen)
{
    size_t limit = strLen <= AQLOG_HASH_CHARMAX ? 0 : (strLen - AQLOG_HASH_CHARMAX);

    uint32_t hash = 5381;

    while (strLen > limit)
    {
        strLen--;
        if (AQLOG_HASH_ISEND(str[strLen]))
        {
            return hash;
        }

        hash = str[strLen] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}




}
//=============================== End of File ==================================
