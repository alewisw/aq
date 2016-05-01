#ifndef MUTEX_H
#define MUTEX_H
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
class Mutex
{
public:

    // Constructs new mutex.
    Mutex(void)
    {
        m_hMutex = CreateMutex(NULL, FALSE, NULL);
    }

    // Destroys this mutex.
    virtual ~Mutex(void)
    {
        CloseHandle(m_hMutex);
    }

    // Not implmented - cannot be copied or assigned.
    Mutex(const Mutex& other);
    Mutex& operator=(const Mutex& other);

    // Locks this mutex.
    void lock(void)
    {
        WaitForSingleObject(m_hMutex, INFINITE);
    }

    // Unlocks this mutex.
    void unlock(void)
    {
        ReleaseMutex(m_hMutex);
    }

private:

    // The mutex handle.
    HANDLE m_hMutex;

};



#endif
//=============================== End of File ==================================
