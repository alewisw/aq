//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"

#include "AQTest.h"
#include "TestPointAction.h"




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtSharedMemory);

//------------------------------------------------------------------------------
AQTEST(given_MemoryBuffer_when_ConstructExternMemory_then_SizeBaseAddressCorrect)
{
    unsigned char mem[1000];
    AQExternMemory emem(mem, sizeof(mem));

    REQUIRE(emem.baseAddress() == mem);
    REQUIRE(emem.size() == 1000);
}

//------------------------------------------------------------------------------
AQTEST(given_ExternMemory_when_WindowInRange_then_WindowAddressCorrect)
{
    unsigned char mem[1000];
    AQExternMemory emem(mem, sizeof(mem));

    AQSharedMemoryWindow wmem(emem, 7, 899);

    REQUIRE(wmem.size() == 899);
    REQUIRE(wmem.baseAddress() == &mem[7]);
}

//------------------------------------------------------------------------------
AQTEST(given_ExternMemory_when_WindowAtMinEnd_then_WindowAddressCorrect)
{
    unsigned char mem[1000];
    AQExternMemory emem(mem, sizeof(mem));

    AQSharedMemoryWindow wmem(emem, 0, 100);

    REQUIRE(wmem.size() == 100);
    REQUIRE(wmem.baseAddress() == &mem[0]);
}

//------------------------------------------------------------------------------
AQTEST(given_ExternMemory_when_WindowAtMaxEnd_then_WindowAddressCorrect)
{
    unsigned char mem[1000];
    AQExternMemory emem(mem, sizeof(mem));

    AQSharedMemoryWindow wmem(emem, 900, 100);

    REQUIRE(wmem.size() == 100);
    REQUIRE(wmem.baseAddress() == &mem[900]);
}

//------------------------------------------------------------------------------
AQTEST(given_ExternMemory_when_WindowSize1ByteAfterEnd_then_LengthErrorException)
{
    unsigned char mem[1000];
    AQExternMemory emem(mem, sizeof(mem));

    REQUIRE_EXCEPTION(AQSharedMemoryWindow wmem(emem, 900, 101), std::length_error);
}

//------------------------------------------------------------------------------
AQTEST(given_ExternMemory_when_WindowOffset1ByteAfterEnd_then_LengthErrorException)
{
    unsigned char mem[1000];
    AQExternMemory emem(mem, sizeof(mem));

    REQUIRE_EXCEPTION(AQSharedMemoryWindow wmem(emem, 1000, 1), std::out_of_range);
}




//=============================== End of File ==================================
