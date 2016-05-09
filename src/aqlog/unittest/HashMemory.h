#ifndef HASHMEMORY_H
#define HASHMEMORY_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLog.h"

#include "AQExternMemory.h"



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

// Used to define a fixed region of hash memory for use in a test case.
class HashMemory : public AQExternMemory
{
public:

    // Constructs the hash memory and binds it to the hash global variable.
    HashMemory(void);

    // Destroys the hash memory and unbinds it from the hash global variable.
    ~HashMemory(void);

private:

    // The actual hash memory to use.
    uint32_t m_words[AQLOG_HASH_TABLE_WORDS];
};




#endif
//=============================== End of File ==================================
