#ifndef AQPROVIDER_H
#define AQPROVIDER_H
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




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQReader;
class AQWriter;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines the MPAC queue provider.
class AQProvider : public IQueueProvider
{
public:

    // Creates an MPAC queue provider with each page being 1 << 'pageSizeShift'
    // bytes in size with at least 'pageCount' pages available.
    AQProvider(int pageSizeShift, size_t pageCount);

    // Destructor for the queue provider.
    virtual ~AQProvider(void);

    // Returns the one reader object for the queue.
    virtual IAQReader& reader(void) { return *m_ireader; }

    // Returns the one writer object for the queue.
    virtual IAQWriter& writer(void) { return *m_iwriter; }

    // Returns the number of pages that can be used at the same time from this provider.
    virtual size_t usablePageCount(void) const { return m_pageCount - 1; }

    // Returns the size of each page in this provider.
    virtual size_t pageSize(void) const { return 1 << m_pageSizeShift; }

    // Called before the test is run to setup the test data, format the queues, and
    // so forth.
    virtual void before(void);

    // Called before each iteration of the test.  This must reset the queue to its
    // empty state.
    virtual void beforeIteration(void);

    // Obtains a string description of this configuration.
    virtual std::string config(void);

    // Obtains a string description of the queue state.
    virtual std::string results(void);

private:

    // The page size shift that has been configured.
    int m_pageSizeShift;

    // The minimum number of pages to provide.
    size_t m_pageCount;

    // The memory for this queue provider.
    unsigned char *m_mem;

    // The memory size for this queue provider.
    size_t m_memSize;

    // The one and only reader from this queue.
    IAQReader *m_ireader;

    // The one and only writer to this queue.
    IAQWriter *m_iwriter;

    // The one and only reader from this queue.
    AQReader *m_aqReader;

    // The one and only writer to this queue.
    AQWriter *m_aqWriter;

};




#endif
//=============================== End of File ==================================
