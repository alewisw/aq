#ifndef IAQSHAREDMEMORY_H
#define IAQSHAREDMEMORY_H
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
#include <stdlib.h>




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
 * Defines an interface that shared memory regions can use to expose their 
 * address and size.
 *
 * Some shared memory implementations will simply allocate memory on the local
 * heap, while others could use a memory mapped file or operating system 
 * supported shared memory.
 */
class IAQSharedMemory
{
protected:

    /**
     * Constructs a new IAQSharedMemory object.
     */
    IAQSharedMemory(void) { }

public:

    /**
    * Destroys this IAQSharedMemory object.
    */
    virtual ~IAQSharedMemory(void) { }

    /**
     * Obtains the base address of the shared memory.
     *
     * @returns The base address of the shared memory or NULL if the shared
     * memory is not available.
     */
    virtual void *baseAddress(void) const = 0;

    /**
     * Obtains the size of the shared memory region.  The first byte is the one
     * pointed to by baseAddress() with the last byte at baseAddress() + size() - 1.
     *
     * @returns The size of the shared memory in bytes.
     */
    virtual size_t size(void) const = 0;

};




#endif
//=============================== End of File ==================================
