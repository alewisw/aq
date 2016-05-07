#ifndef AQSHAREDMEMORYWINDOW_H
#define AQSHAREDMEMORYWINDOW_H
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
 * Provides a limited window into a shared memory region.  This is effectivy
 * a subregion of an existing shared memory region.
 */
class AQSharedMemoryWindow : public IAQSharedMemory
{
public:

    /**
     * Constructs a new AQSharedMemoryWindow object that provides a window
     * into another shared memory object.
     *
     * @param sm The other shared memory object.  The sm object must remain
     * valid and in scope so long as this object is valid and in scope.
     * @param off The offset into the other shared memory object.
     * @param size The size of the region in the other shared memory objet
     * to provide a window into.
     * @throws std::out_of_range If off is greater than or equal to the size
     * of the passed shared memory region.
     * @throws std::length_error If off plus size is greater than the size 
     * of the passed shared memory region.
     */
    AQSharedMemoryWindow(IAQSharedMemory& sm, size_t off, size_t size);

    /**
    * Destroys this AQSharedMemoryWindow object.
    */
    virtual ~AQSharedMemoryWindow(void);

protected:
    // Prevent copy construction or assignment of this object.
    AQSharedMemoryWindow(const AQSharedMemoryWindow& other);
    AQSharedMemoryWindow& operator=(const AQSharedMemoryWindow& other);

private:

    // The root shared memory into which this is a window.
    IAQSharedMemory& m_sm;

    // The size of the memory region.
    size_t m_size;

    // The memory region.
    void *m_mem;

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
