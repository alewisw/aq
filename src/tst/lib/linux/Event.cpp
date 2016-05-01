//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Event.h"




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
Event::Event(void)
    : m_state(false)
{
    pthread_mutexattr_t mattr;
    
    pthread_mutexattr_init(&mattr);
    pthread_mutex_init(&m_mutex, &mattr);
    pthread_mutexattr_destroy(&mattr);
    
    pthread_condattr_t cattr;
    
    pthread_condattr_init(&cattr);
    pthread_cond_init(&m_cond, &cattr);
    pthread_condattr_destroy(&cattr);
}

//------------------------------------------------------------------------------
Event::~Event(void)
{
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
}

//------------------------------------------------------------------------------
void Event::set(void)
{
    pthread_mutex_lock(&m_mutex);
    m_state = true;
    pthread_cond_broadcast(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

//------------------------------------------------------------------------------
void Event::reset(void)
{
    pthread_mutex_lock(&m_mutex);
    m_state = false;
    pthread_mutex_unlock(&m_mutex);
}

//------------------------------------------------------------------------------
void Event::block(void)
{
    pthread_mutex_lock(&m_mutex);
    while (!m_state)
    {
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    pthread_mutex_unlock(&m_mutex);
}




//=============================== End of File ==================================
