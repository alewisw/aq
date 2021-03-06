#ifndef EVENT_H
#define EVENT_H
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

// Encapsulates an event that can be used to block a thread until it becomes
// set.
class Event
{
public:

    // Constructs new event.
    Event(void);

    // Destroys this event.
    virtual ~Event(void);

    // Not implmented - cannot be copied or assigned.
    Event(const Event& other);
    Event& operator=(const Event& other);

    // Resets this event.Locks this mutex.
    void reset(void);

    // Sets this event.
    void set(void);

    // Blocks until this event becomes set.
    void block(void);

private:

    // The Windows event handle.
    HANDLE m_hEvent;

};



#endif
//=============================== End of File ==================================
