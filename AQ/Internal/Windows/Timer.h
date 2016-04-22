#ifndef TIMER_H
#define TIMER_H
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

// Defines functions for measuring the passing of time.
namespace aq { class Timer
{
private:

    // Constructor is not defined - this is a utility class.
    Timer(void);

public:

    // Represents a millisecond timer value.
    typedef DWORD Ms_t;

    // Returns the current millisecond timer - used to start timing a duration.
    static Ms_t start(void)
    {
#ifdef AQ_TEST_UNIT
        return m_fixClock ? m_fixClockMs : GetTickCount();
#else
        return GetTickCount();
#endif
    }

    // Returns the number of milliseconds that have elapsed since a starting time.
    static Ms_t elapsed(Ms_t startMs)
    {
        return start() - startMs;
    }

    // Sleeps for the given period in milliseconds.
    static void sleep(Ms_t ms)
    {
#ifdef AQ_TEST_UNIT
        if (m_fixClock)
        {
            m_fixClockMs += ms;
        }
        else
        {
            Sleep(ms);
        }
#else
        Sleep(ms);
#endif
    }

    // Sets the clock to return the fixed value 'ms'.  Used for unit tests.
#ifdef AQ_TEST_UNIT
    static void fixClock(Ms_t ms)
    {
        m_fixClock = true;
        m_fixClockMs = ms;
    }

private:

    // Set to true if the clock value has been fixed.
    static bool m_fixClock;

    // The fixed clock value in miliseconds.
    static Ms_t m_fixClockMs;
#endif

};}




#endif
//=============================== End of File ==================================
