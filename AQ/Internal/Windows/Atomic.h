#ifndef ATOMIC_H
#define ATOMIC_H
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
#include <intrin.h>

#include <cstdint>

#pragma intrinsic(_InterlockedOr)




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

// Defines atomic operations that are safe across threads and processes.
class Atomic
{
private:

    // Constructor is not defined - this is a utility class.
    Atomic(void);

public:

    // Performs an atomic compare-and-exchange operation on the specified 
    // values. The function compares two specified 32-bit values and exchanges
    // with another 32-bit value based on the outcome of the comparison.
    //
    // If '*dest' is equal to 'comparand' then '*dest' is set to 'exchange'
    // otherwise '*dest' remains unchanged.  Returns the value read from '*dest';
    // thus only if the returned value equals 'comparand' the change was made.
    static uint32_t cmpXchg(volatile uint32_t *dest, uint32_t exchange, uint32_t comparand)
    {
        volatile LONG *ldest = (volatile LONG *)dest;
        LONG lexchange = (LONG)exchange;
        LONG lcomparand = (LONG)comparand;

        return (uint32_t)InterlockedCompareExchange(ldest, lexchange, lcomparand);
    }

    // Peforms an atomic increment by '1' on 'dest'.
    static uint64_t increment(volatile uint64_t *dest)
    {
        volatile LONGLONG *ldest = (volatile LONGLONG *)dest;

        return InterlockedIncrement64(ldest);
    }

    // Peforms an atomic increment by '1' on 'dest'.
    static uint32_t increment(volatile uint32_t *dest)
    {
        volatile LONG *ldest = (volatile LONG *)dest;

        return InterlockedIncrement(ldest);
    }

    // Performs an atomic read of the passed memory location, returning the 
    // vaue that was read.
    static uint32_t read(volatile uint32_t *src)
    {
        volatile LONG *lsrc = (volatile LONG *)src;
        return (uint32_t)_InterlockedOr(lsrc, 0);
    }

    // Atomically Sets the value located at 'dest' to 'value'.
    static void write(volatile uint32_t *dest, uint32_t value)
    {
        volatile LONG *ldest = (volatile LONG *)dest;
        LONG lvalue = (LONG)value;

        InterlockedExchange(ldest, lvalue);
    }

    // Performs an atomic 'OR' of the passed memory location with the bits in 
    // 'mask'.
    static void bitwiseOr(volatile uint32_t *dest, uint32_t mask)
    {
        volatile LONG *ldest = (volatile LONG *)dest;
        LONG lmask = (LONG)mask;
        _InterlockedOr(ldest, mask);
    }

};



#endif
//=============================== End of File ==================================
