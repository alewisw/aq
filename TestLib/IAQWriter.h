#ifndef IAQWRITER_H
#define IAQWRITER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQWriterItem.h"




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

// Interface to an Allocating Queue writer implementation.  Used for generalising 
// test cases - not for general usage.
class IAQWriter
{
public:

    // Constructor and destructor stubs for this interface.
    IAQWriter(void) { };
    virtual ~IAQWriter(void) { };

    // See AQ and AQWriter classes for the definition of these functions.
    virtual size_t pageSize(void) const = 0;
    virtual bool claim(AQWriterItem& item, size_t memSize) = 0;
    virtual bool commit(AQWriterItem& item) = 0;

};

// Template for implementing an IAQWriter.
template<typename T> class TAQWriter : public IAQWriter
{
public:
    TAQWriter(T& aq) : m_aq(aq) { };
    virtual ~TAQWriter(void) { };

    virtual size_t pageSize(void) const { return m_aq.pageSize(); }
    virtual bool claim(AQWriterItem& item, size_t memSize) { return m_aq.claim(item, memSize); }
    virtual bool commit(AQWriterItem& item) { return m_aq.commit(item); };

private:

    T& m_aq;

};




#endif
//=============================== End of File ==================================
