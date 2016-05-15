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
        return 0;
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
