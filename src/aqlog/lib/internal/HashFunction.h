#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H
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




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Prototype function used to calculate a hash of the string 'str' of length
// 'strLen'.
typedef uint32_t (*HashFunction_fn)(uint32_t mask, const char *str, size_t strLen, bool zeroLengthHashToZero);




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// The static class that contains all the hash functions.
namespace aqlog { class HashFunction
{
public:

    // Implements the current standard hash function used by the AQLog library.
    static uint32_t standard(uint32_t mask, const char *str, size_t strLen, bool zeroLengthHashToZero);

    // Implements the djb2a hash function.
    //
    // This algorithm (k = 33) was first reported by dan bernstein many years ago in 
    // comp.lang.c.  The equation is hash(i) = hash(i - 1) * 33 + char.
    static uint32_t djb2(uint32_t mask, const char *str, size_t strLen, bool zeroLengthHashToZero);

    // Implements the djb2b hash function.
    //
    // This is an update to the djb2 algorithm that changes the addition to an xor.
    // The equation is hash(i) = (hash(i - 1) * 33) ^ char.
    static uint32_t djb2a(uint32_t mask, const char *str, size_t strLen, bool zeroLengthHashToZero);

    // Implements the sdbm hash function.
    //
    // This uses the equation hash(i) = hash(i - 1) * 65599 + char.
    static uint32_t sdbm(uint32_t mask, const char *str, size_t strLen, bool zeroLengthHashToZero);
    

};}




#endif
//=============================== End of File ==================================
