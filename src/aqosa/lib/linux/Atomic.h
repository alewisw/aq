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

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Define this to relay on GCC __sync operations exclusivly.
#define AQ_ATOMIC_ALWAYS_USE_SYNC




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
namespace aqosa { class Atomic
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
        return __sync_val_compare_and_swap (dest, comparand, exchange);
    }

    // Peforms an atomic increment by '1' on 'dest'.
    static inline uint64_t increment(volatile uint64_t *dest)
    {
        return __sync_add_and_fetch(dest, 1);
    }

    // Peforms an atomic increment by '1' on 'dest'.
    static inline uint32_t increment(volatile uint32_t *dest)
    {
        return __sync_add_and_fetch(dest, 1);
    }

    // Performs an atomic read of the passed memory location, returning the 
    // vaue that was read.
    //
    // Implementation relies on (1) to guarantee atomicity and (2) to
    // guarantee ordering.
    static inline uint32_t read(volatile uint32_t *src)
    {
#ifdef AQ_ATOMIC_ALWAYS_USE_SYNC
        return __sync_add_and_fetch(src, 0);
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
#ifdef AQ_ATOMIC_ALWAYS_USE_SYNC
        __sync_synchronize();
        *dest = value;
        __sync_synchronize();
#else
        *dest = value;
#endif
    }

    // Performs an atomic 'OR' of the passed memory location with the bits in 
    // 'mask'.
    static inline void bitwiseOr(volatile uint32_t *dest, uint32_t mask)
    {
        __sync_or_and_fetch(dest, mask);
    }

};};



#endif
//=============================== End of File ==================================
