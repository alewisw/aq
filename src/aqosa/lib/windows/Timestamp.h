#ifndef TIMESTAMP_H
#define TIMESTAMP_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <Windows.h>

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Used to obtain a timestamp.
namespace aqosa { class Timestamp
{
private:

    // Constructor is not defined - this is a utility class.
    Timestamp(void);

public:

    // Returns the UNIX timestamp in nanoseconds since 1 January 1970.
    static inline uint64_t now(void)
    {
#ifdef AQ_TEST_UNIT
        if (m_fixTimestamp != 0)
        {
            return m_fixTimestamp;
        }
#endif
        FILETIME ft;

        GetSystemTimeAsFileTime(&ft);

        // FILETIME is a 64-bit value representing the number of 100-nanosecond 
        // intervals since January 1, 1601 (UTC).
        //
        // We need to convert into number of nanoseconds since 1970.  First construct
        // it as a uint64_t.
        uint64_t t = (uint64_t)ft.dwLowDateTime | ((uint64_t)ft.dwHighDateTime << 32);

        // Now move to offset from 1970.  Constant taken from:
        //  https://msdn.microsoft.com/en-us/library/windows/desktop/ms724228(v=vs.85).aspx
        t -= 116444736000000000ULL;

        // Finally convert to nanoseconds before returning (is in 100's of nano
        // seconds so multiply by 100).
        return t * 100ULL;
    }

    // Sets the clock to return the fixed value 'ms'.  Used for unit tests.
#ifdef AQ_TEST_UNIT
    static void fixTimestamp(uint64_t ts)
    {
        m_fixTimestamp = ts;
    }

private:

    // Set to non-zero to give a fixed timestamp.
    static uint64_t m_fixTimestamp;
#else
public:
    static void fixTimestamp(uint64_t ts) { }

#endif


};}




#endif
//=============================== End of File ==================================
