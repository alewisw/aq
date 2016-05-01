#ifndef PRNG_H
#define PRNG_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------




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

// Encapsulates a simple pseudo-random number generator.  The generator is 
// completly predictable - given the same starting seed it will always generate
// the same seqeunce of numbers.
class Prng
{
public:

    // Constructs a new record generator which seeds based on the thread number.
    Prng(unsigned int seed);

    // Creats an exact copy of this record generator including its current PRNG
    // state.
    Prng(const Prng& other);

    // Assigns the value of this record generator to exactly match another.
    Prng& operator=(const Prng& other);

    // Destroys this record generator.
    virtual ~Prng(void);

private:

    // The current PRNG value.
    unsigned long long m_prng;

public:

    // Gets the current m_prng value.
    unsigned int get(void) const;

    // Advances the m_prng field to the next value in the sequence.
    unsigned int next(void);

};



#endif
//=============================== End of File ==================================
