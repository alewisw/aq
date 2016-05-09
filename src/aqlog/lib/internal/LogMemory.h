#ifndef LOGMEMORY_H
#define LOGMEMORY_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQSharedMemoryWindow.h"

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class IAQSharedMemory;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// A LogMemory object is used to encapsulate the require memory regions for the
// log based on a single input shared memory region.
namespace aqlog { class LogMemory
{
public:

    // Constructs a new log memory region that divides up the passed 'sm' 
    // instance.  If the passed shared memory region is not large enough
    // a length_error exception is thrown.
    LogMemory(IAQSharedMemory& sm);

private:

    // Calcuates the size for the AQ memory region.
    static size_t calculateAqMemorySize(IAQSharedMemory& sm);

public:

    // Destroys this log memory region.
    ~LogMemory(void);

private:
    // No implementation provided - log memory regions cannot be copied or 
    // assigned.
    LogMemory(const LogMemory& other);
    LogMemory& operator=(const LogMemory& other);

private:

    // The memory to be used by the allocating queue.
    AQSharedMemoryWindow m_aqMemory;

    // The memory to be used by the log level hash.
    AQSharedMemoryWindow m_logLevelHashMemory;

public:

    // Obtains the memory region to be used by the allocating queue.
    IAQSharedMemory& aqMemory(void) { return m_aqMemory; }

    // Obtains the memory region to be used by the log level hash.
    IAQSharedMemory& logLevelHashMemory(void) { return m_logLevelHashMemory; }

};}




#endif
//=============================== End of File ==================================
