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

#include <stdint.h>

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
namespace aqosa { class Timer
{
private:

    // Constructor is not defined - this is a utility class.
    Timer(void);

public:

    // Returns the current millisecond timer - used to start timing a duration.
    static uint32_t start(void)
    {
#ifdef AQ_TEST_UNIT
        return m_fixClock ? m_fixClockMs : (uint32_t)GetTickCount();
#else
        return (uint32_t)GetTickCount();
#endif
    }

    // Returns the number of milliseconds that have elapsed since a starting time.
    static uint32_t elapsed(uint32_t startMs)
    {
        return start() - startMs;
    }

    // Sleeps for the given period in milliseconds.
    static void sleep(uint32_t ms)
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
    static void fixClock(uint32_t ms)
    {
        m_fixClock = true;
        m_fixClockMs = ms;
    }

private:

    // Set to true if the clock value has been fixed.
    static bool m_fixClock;

    // The fixed clock value in miliseconds.
    static uint32_t m_fixClockMs;
#endif

};}




#endif
//=============================== End of File ==================================
