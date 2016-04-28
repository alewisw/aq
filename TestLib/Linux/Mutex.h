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

#include <pthread.h>




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
        pthread_mutexattr_t mattr;
        
        pthread_mutexattr_init(&mattr);
        pthread_mutex_init(&m_mutex, &mattr);
        pthread_mutexattr_destroy(&mattr);
    }

    // Destroys this mutex.
    virtual ~Mutex(void)
    {
        pthread_mutex_destroy(&m_mutex);
    }

    // Not implmented - cannot be copied or assigned.
    Mutex(const Mutex& other);
    Mutex& operator=(const Mutex& other);

    // Locks this mutex.
    void lock(void)
    {
        pthread_mutex_lock(&m_mutex);
    }

    // Unlocks this mutex.
    void unlock(void)
    {
        pthread_mutex_unlock(&m_mutex);
    }

private:

    // The mutex handle.
    pthread_mutex_t m_mutex;

};



#endif
//=============================== End of File ==================================
