//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQProvider.h"

#include "AQHeapMemory.h"
#include "AQReader.h"
#include "AQWriter.h"

#include "CtrlOverlay.h"

#include "IAQReader.h"
#include "IAQWriter.h"

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
AQProvider::AQProvider(int pageSizeShift, size_t pageCount)
    : m_pageSizeShift(pageSizeShift)
    , m_pageCount(pageCount)
{
    size_t memSize = sizeof(CtrlOverlay) + sizeof(uint32_t) * pageCount
                                         + (pageCount << pageSizeShift); 
    
    size_t i = 1;
    do
    {
        m_mem = new AQHeapMemory(memSize);
        m_aqReader = new AQReader(*m_mem);
        m_aqReader->format(pageSizeShift, COMMIT_TIMEOUT_MS);
        if (m_aqReader->pageCount() < pageCount)
        {
            delete m_aqReader;
            m_aqReader = NULL;
            delete m_mem;
            m_mem = NULL;
            memSize += i << pageSizeShift;
            i++;
        }
    } while (m_aqReader == NULL);
    m_aqWriter = new AQWriter(*m_mem);

    m_ireader = new TAQReader<AQReader>(*m_aqReader);
    m_iwriter = new TAQWriter<AQWriter>(*m_aqWriter);
}

//------------------------------------------------------------------------------
AQProvider::~AQProvider(void)
{
    delete m_ireader;
    delete m_iwriter;
    delete m_aqReader;
    delete m_aqWriter;
    delete m_mem;
}

//------------------------------------------------------------------------------
void AQProvider::before(void)
{

}

//------------------------------------------------------------------------------
void AQProvider::beforeIteration(void)
{
    m_aqReader->format(m_pageSizeShift, COMMIT_TIMEOUT_MS);
}

//------------------------------------------------------------------------------
std::string AQProvider::config(void)
{
    ostringstream ss;

    ss << m_aqReader->pageCount() << " pages @ " << m_aqReader->pageSize() << " bytes";

    return ss.str();
}

//------------------------------------------------------------------------------
std::string AQProvider::results(void)
{
    ostringstream ss;

    ss << "contention[" << m_aqReader->claimContentionCount() << "]";

    return ss.str();
}




//=============================== End of File ==================================
