#ifndef STOPWATCH_H
#define STOPWATCH_H
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
#include <windows.h>




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

// A stopwatch is used to measure elapsed time.
class Stopwatch
{
public:

    // Constructs new stopwatch.
    Stopwatch(void) : m_start(GetTickCount()) { }

    // Copy constructor.
    Stopwatch(const Stopwatch& other) : m_start(other.m_start) { }

    // Assignment operator.
    Stopwatch& operator=(const Stopwatch& other)
    {
        if (this != &other)
        {
            m_start = other.m_start;
        }
        return *this;
    }

    // Destroys this stopwatch.
    ~Stopwatch(void) { } 

private:

    // The start time.
    uint32_t m_start;

public:

    // Returns the elapsed time in seconds.
    double elapsedSecs(void) const
    {
        uint32_t ms = GetTickCount() - m_start;

        return (double)ms / 1000.0;
    }
    
};



#endif
//=============================== End of File ==================================
