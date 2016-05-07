//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQSharedMemoryWindow.h"

#include <stdexcept>

using namespace std;




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
AQSharedMemoryWindow::AQSharedMemoryWindow(IAQSharedMemory& sm, size_t off,
    size_t size)
    : m_sm(sm)
    , m_mem((unsigned char *)sm.baseAddress() + off)
    , m_size(size)
{
    if (off >= sm.size())
    {
        throw out_of_range("Offset for AQSharedMemoryWindows is beyond the "
                           "size of the passed IAQSharedMemory object");
    }
    if (off + size > sm.size())
    {
        throw length_error("Offset + size for AQSharedMemoryWindows is beyond the "
                           "size of the passed IAQSharedMemory object");
    }
}

//------------------------------------------------------------------------------
AQSharedMemoryWindow::~AQSharedMemoryWindow(void)
{

}




//=============================== End of File ==================================
