#ifndef AQSTRAWMANPROVIDER_H
#define AQSTRAWMANPROVIDER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "IQueueProvider.h"

#include "AQStrawMan.h"

#include <sstream>




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

// Defines the MPAC queue provider.
template <typename TLock> class AQStrawManProvider : public IQueueProvider
{
public:

    // Creates an allocating deque provider with each page being 1 << 'pageSizeShift'
    // bytes in size with at least 'pageCount' pages available.
    AQStrawManProvider(int pageSizeShift, size_t pageCount) 
        : m_deque(pageSizeShift, pageCount)
        , m_reader(m_deque)
        , m_writer(m_deque)
    {
    }

    // Destructor for the queue provider.
    virtual ~AQStrawManProvider(void) { }

    // Returns the one reader object for the queue.
    virtual IAQReader& reader(void) { return m_reader; }

    // Returns the one writer object for the queue.
    virtual IAQWriter& writer(void) { return m_writer; }

    // Returns the number of pages that can be used at the same time from this provider.
    virtual size_t usablePageCount(void) const { return m_deque.pageCount(); }

    // Returns the size of each page in this provider.
    virtual size_t pageSize(void) const { return m_deque.pageSize(); }

    // Called before the test is run to setup the test data, format the queues, and
    // so forth.
    virtual void before(void) { }

    // Called before each iteration of the test.  This must reset the queue to its
    // empty state.
    virtual void beforeIteration(void) { m_deque.reset(); }

    // Obtains a string description of this configuration.
    virtual std::string config(void)
    {
        std::ostringstream ss;

        ss << m_deque.pageCount() << " pages @ " << m_deque.pageSize() << " bytes";

        return ss.str();
    }


private:

    // The allocating deque.
    AQStrawMan<TLock> m_deque;

    // The reader and writer interfaces.
    TAQReader< AQStrawMan<TLock> > m_reader;
    TAQWriter< AQStrawMan<TLock> > m_writer;

};




#endif
//=============================== End of File ==================================
