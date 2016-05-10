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

// Require at least this many bytes in the static buffer to try printing into
// it strftime().
#define STRFTIME_SCOPY_MIN_SIZE         20

// The base size of the memory to allocated for dynamic strftime generation.
#define STRFTIME_DCOPY_BASE_SIZE        4096




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
    va_list cargp = argp;
    int count = _vscprintf(fmt, cargp);
    if (count > 0)
    {
        size_t len = (size_t)count;
        char *str = allocString(len + 1);
        _vsnprintf_s(str, len + 1, len + 1, fmt, argp);
        appendPointer(str, len);
    }
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::appendftime(const char *fmt, time_t time)
{
    struct tm tm;
    if (0 == localtime_s(&tm, &time))
    {
        appendftime(fmt, &tm);
    }
    return *this;
}

//------------------------------------------------------------------------------
size_t AQLogStringBuilder::vinsertf(const iterator& pos, const char *fmt, va_list argp)
{
    size_t len;
    va_list cargp = argp;
    int count = _vscprintf(fmt, cargp);
    if (count > 0)
    {
        len = (size_t)count;
        char *str = allocString(len + 1);
        _vsnprintf_s(str, len + 1, len + 1, fmt, argp);
        insertPointer(pos, str, len);
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
    if (0 == localtime_s(&tm, &time))
    {
        s = insertftime(pos, fmt, &tm);
    }
    return s;
}




//=============================== End of File ==================================
