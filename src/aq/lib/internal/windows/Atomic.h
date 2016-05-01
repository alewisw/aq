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

#include <stdint.h>

#pragma intrinsic(_InterlockedOr)




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Define this to relay on Windows InterlockedXYZ() operations exclusivly.
//#define AQ_ATOMIC_ALWAYS_USE_INTERLOCKED




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
//
// We relay on two statements from Microsoft to guarantee correctness; namely:
//
// (1) According to the MSDN 'Interlocked Variable Access' page:
//         https://msdn.microsoft.com/en-us/library/windows/desktop/ms684122(v=vs.85).aspx
//     Simple reads and writes to properly-aligned 32-bit variables are 
//     atomic operations. In other words, you will not end up with only one
//     portion of the variable updated; all bits are updated in an atomic
//     fashion.
//
// (2) According to the MSDN 'MemoryBarrier()' page:
//         https://msdn.microsoft.com/en-us/library/windows/desktop/ms684208(v=vs.85).aspx
//     With Visual Studio 2003, volatile to volatile references are ordered;
//     the compiler will not re-order volatile variable access.  With Visual
//     Studio 2005, the compiler also uses acquire semantics for read 
//     operations on volatile variables and release semantics for write 
//     operations on volatile variables (when supported by the CPU).
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
    static inline uint32_t cmpXchg(volatile uint32_t *dest, uint32_t exchange, uint32_t comparand)
    {
        volatile LONG *ldest = (volatile LONG *)dest;
        LONG lexchange = (LONG)exchange;
        LONG lcomparand = (LONG)comparand;

        return (uint32_t)InterlockedCompareExchange(ldest, lexchange, lcomparand);
    }

    // Peforms an atomic increment by '1' on 'dest'.
    static inline uint64_t increment(volatile uint64_t *dest)
    {
        volatile LONGLONG *ldest = (volatile LONGLONG *)dest;

        return InterlockedIncrement64(ldest);
    }

    // Peforms an atomic increment by '1' on 'dest'.
    static inline uint32_t increment(volatile uint32_t *dest)
    {
        volatile LONG *ldest = (volatile LONG *)dest;

        return InterlockedIncrement(ldest);
    }

    // Performs an atomic read of the passed memory location, returning the 
    // vaue that was read.
    //
    // Implementation relies on (1) to guarantee atomicity and (2) to
    // guarantee ordering.
    static inline uint32_t read(volatile uint32_t *src)
    {
#ifdef AQ_ATOMIC_ALWAYS_USE_INTERLOCKED
        volatile LONG *lsrc = (volatile LONG *)src;
        return (uint32_t)_InterlockedOr(lsrc, 0);
#else
        return *src;
#endif
    }

    // Atomically sets the value located at 'dest' to 'value'.
    //
    // Implementation relies on (1) to guarantee atomicity and (2) to
    // guarantee ordering.
    static inline void write(volatile uint32_t *dest, uint32_t value)
    {
#ifdef AQ_ATOMIC_ALWAYS_USE_INTERLOCKED
        volatile LONG *ldest = (volatile LONG *)dest;
        LONG lvalue = (LONG)value;

        InterlockedExchange(ldest, lvalue);
#else
        *dest = value;
#endif
    }

    // Performs an atomic 'OR' of the passed memory location with the bits in 
    // 'mask'.
    static inline void bitwiseOr(volatile uint32_t *dest, uint32_t mask)
    {
        volatile LONG *ldest = (volatile LONG *)dest;
        LONG lmask = (LONG)mask;
        _InterlockedOr(ldest, lmask);
    }

};



#endif
//=============================== End of File ==================================
