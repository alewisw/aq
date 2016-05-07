#ifndef AQHEAPMEMORY_H
#define AQHEAPMEMORY_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "IAQSharedMemory.h"




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

/**
 * Provides a shared memory region allocated on the local process heap using
 * 'new'.
 */
class AQHeapMemory : public IAQSharedMemory
{
public:

    /**
     * Constructs a new AQHeapMemory object of a particular size.
     *
     * @param size The size in bytes of the heap shared memory region.
     */
    AQHeapMemory(size_t size);

    /**
    * Destroys this AQHeapMemory object.
    */
    virtual ~AQHeapMemory(void);

protected:
    // Prevent copy construction or assignment of this object.
    AQHeapMemory(const AQHeapMemory& other);
    AQHeapMemory& operator=(const AQHeapMemory& other);

private:

    // The size of the memory region.
    size_t m_size;

    // The memory region.
    unsigned long long *m_mem;

public:

    /**
    * Obtains the base address of the shared memory.
    *
    * @returns The base address of the shared memory or NULL if the shared
    * memory is not available.
    */
    virtual void *baseAddress(void) const { return m_mem; }

    /**
    * Obtains the size of the shared memory region.  The first byte is the one
    * pointed to by baseAddress() with the last byte at baseAddress() + size() - 1.
    *
    * @returns The size of the shared memory in bytes.
    */
    virtual size_t size(void) const { return m_size; }

};




#endif
//=============================== End of File ==================================
