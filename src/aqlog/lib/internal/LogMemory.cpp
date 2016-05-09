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

#include "LogMemory.h"

#include <sstream>
#include <stdexcept>

using namespace std;

namespace aqlog
{



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
LogMemory::LogMemory(IAQSharedMemory& sm)
    : m_aqMemory(sm, 0, calculateAqMemorySize(sm))
    , m_logLevelHashMemory(sm, m_aqMemory.size(), AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t))
{

}

//------------------------------------------------------------------------------
size_t LogMemory::calculateAqMemorySize(IAQSharedMemory& sm)
{
    // Align the memory.
    size_t msize = sm.size() & ~0x3;

    if (msize < AQLOG_SHM_MINIMUM_SIZE)
    {
        ostringstream ss;

        ss << "The log shared memory must be at least " << msize 
           << " bytes in size, however the provided shared memory region was only " << sm.size();
        throw length_error(ss.str());
    }

    return msize - AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t);
}

//------------------------------------------------------------------------------
LogMemory::~LogMemory(void)
{

}



}
//=============================== End of File ==================================
