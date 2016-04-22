//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQReferenceProvider.h"

#include "CtrlOverlay.h"

#include <sstream>

using namespace std;
using namespace aq;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// The commit timeout to use.
#define COMMIT_TIMEOUT_MS               30000




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
AQReferenceProvider::AQReferenceProvider(int pageSizeShift, size_t pageCount)
    : m_deque(pageSizeShift, pageCount)
{
}

//------------------------------------------------------------------------------
AQReferenceProvider::~AQReferenceProvider(void)
{
}

//------------------------------------------------------------------------------
void AQReferenceProvider::before(void)
{

}

//------------------------------------------------------------------------------
void AQReferenceProvider::beforeIteration(void)
{
    m_deque.reset();
}

//------------------------------------------------------------------------------
std::string AQReferenceProvider::config(void)
{
    ostringstream ss;

    ss << m_deque.pageCount() << " pages @ " << m_deque.pageSize() << " bytes";

    return ss.str();
}




//=============================== End of File ==================================
