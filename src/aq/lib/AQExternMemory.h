#ifndef AQEXTERNMEMORY_H
#define AQEXTERNMEMORY_H
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
 * Provides a shared memory region where the actual memory is allocated and
 * managed externally.
 */
class AQExternMemory : public IAQSharedMemory
{
public:

    /**
     * Constructs a new external memory region.
     *
     * @param mem The memory base address.
     * @param memSize The size of the memory region.
     */
    AQExternMemory(void *mem, size_t memSize) : m_mem(mem), m_memSize(memSize) { }

    /**
    * Destroys this AQExternMemory object.
    */
    virtual ~AQExternMemory(void) { }

protected:
    // Prevent copy construction or assignment of this object.
    AQExternMemory(const AQExternMemory& other);
    AQExternMemory& operator=(const AQExternMemory& other);

private:

    // The base memory address.
    void *m_mem;

    // The memory size.
    size_t m_memSize;

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
    virtual size_t size(void) const { return m_memSize; }

};




#endif
//=============================== End of File ==================================
