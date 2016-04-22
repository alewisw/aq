#ifndef CRITICALSECTION_H
#define CRITICALSECTION_H
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

// Encapsulates a critical section or mutex for thread synchronization.
class CriticalSection
{
public:

    // Constructs new mutex.
    CriticalSection(void)
    {
        InitializeCriticalSection(&m_criticalSection);
    }

    // Destroys this mutex.
    virtual ~CriticalSection(void)
    {
        DeleteCriticalSection(&m_criticalSection);
    }

    // Not implmented - cannot be copied or assigned.
    CriticalSection(const CriticalSection& other);
    CriticalSection& operator=(const CriticalSection& other);

    // Locks this mutex.
    void lock(void)
    {
        EnterCriticalSection(&m_criticalSection);
    }

    // Unlocks this mutex.
    void unlock(void)
    {
        LeaveCriticalSection(&m_criticalSection);
    }

private:

    // The mutex handle.
    CRITICAL_SECTION m_criticalSection;

};



#endif
//=============================== End of File ==================================
