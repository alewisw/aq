//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQ.h"
#include "AQItem.h"
#include "AQUnformattedException.h"

#include "CtrlOverlay.h"
#include "TestPointNotifier.h"

#include <sstream>
#include <stddef.h>

using namespace std;
using namespace aq;




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
AQ::AQ(int testPointCount, void *mem, size_t memSize, aq::TraceBuffer *trace)
    : m_ctrl((CtrlOverlay *)mem)
    , m_memSize(memSize)
    , m_trace(trace)
#ifdef AQ_TEST_POINT
    , m_tpn(NULL)
#endif
{
#ifdef AQ_TEST_POINT
    m_tpn = new TestPointNotifier(testPointCount, this);
#endif
}

//------------------------------------------------------------------------------
AQ::AQ(const AQ& other)
    : m_ctrl(other.m_ctrl)
    , m_memSize(other.m_memSize)
    , m_trace(other.m_trace)
#ifdef AQ_TEST_POINT
    , m_tpn(NULL)
#endif
{
#ifdef AQ_TEST_POINT
    m_tpn = new TestPointNotifier(other.m_tpn->testPointCount(), this);
#endif
}

//------------------------------------------------------------------------------
AQ& AQ::operator=(const AQ& other)
{
    if (this != &other)
    {
        m_ctrl = other.m_ctrl;
        m_memSize = other.m_memSize;
        m_trace = other.m_trace;
    }
    return *this;
}

//-----------------------------------------------------------------------------
AQ::~AQ(void)
{
#ifdef AQ_TEST_POINT
    if (m_tpn)
    {
        delete m_tpn;
        m_tpn = NULL;
    }
#endif
}

//------------------------------------------------------------------------------
bool AQ::isFormatted(void) const
{
    if (m_ctrl == NULL || m_memSize <   offsetof(CtrlOverlay, headerXref) 
                                      + sizeof(m_ctrl->headerXref))
    {
        return false;
    }
    else
    {
        return m_ctrl->isFormatted(m_memSize);
    }
}

//------------------------------------------------------------------------------
bool AQ::isExtendable(void) const
{
    return isFormatted() && (m_ctrl->options & OPTION_EXTENDABLE) != 0;
}

//------------------------------------------------------------------------------
size_t AQ::pageSize(void) const
{
    return isFormatted() ? (1 << m_ctrl->pageSizeShift) : 0;
}

//------------------------------------------------------------------------------
size_t AQ::pageCount(void) const
{
    return isFormatted() ? m_ctrl->pageCount : 0;
}

//------------------------------------------------------------------------------
size_t AQ::availableSize(void) const
{
    return m_ctrl->availableSequentialPages(
           m_ctrl->queueRefToIndex(Atomic::read(&m_ctrl->headRef)),
           m_ctrl->queueRefToIndex(Atomic::read(&m_ctrl->tailRef))) << m_ctrl->pageSizeShift;
}

//------------------------------------------------------------------------------
uint32_t AQ::claimContentionCount(void) const
{
    return m_ctrl->claimContention;
}

//------------------------------------------------------------------------------
aq::CtrlOverlay *AQ::ctrlThrowOnUnformatted(const char *func) const
{
    if (isFormatted())
    {
        return m_ctrl;
    }

    ostringstream ss;
    ss << "Cannot access MPAC queue in " << func << ": the queue is unformatted";

    throw AQUnformattedException(ss.str());
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
void AQ::testPoint(int tp)
{
    m_tpn->testPoint(tp);
}
#endif




//=============================== End of File ==================================
