//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogStringBuilder.h"

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




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::vappendf(const char *fmt, va_list argp)
{
    char *str = NULL;
    int count = vasprintf(&str, fmt, argp);
    if (count > 0)
    {
        size_t len = (size_t)count;
        appendFree(str, len);
    }
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::appendftime(const char *fmt, time_t time)
{
    struct tm tm;
    if (NULL != localtime_r(&time, &tm))
    {
        appendftime(fmt, &tm);
    }
    return *this;
}

//------------------------------------------------------------------------------
size_t AQLogStringBuilder::vinsertf(const iterator& pos, const char *fmt, va_list argp)
{
    char *str = NULL;
    size_t len;
    int count = vasprintf(&str, fmt, argp);
    if (count > 0)
    {
        len = (size_t)count;
        insertFree(pos, str, len);
    }
    else
    {
        len = 0;
    }
    return len;
}

//------------------------------------------------------------------------------
size_t AQLogStringBuilder::insertftime(const iterator& pos, const char *fmt,
    time_t time)
{
    struct tm tm;
    size_t s = 0;
    if (NULL != localtime_r(&time, &tm))
    {
        s = insertftime(pos, fmt, &tm);
    }
    return s;
}




//=============================== End of File ==================================
