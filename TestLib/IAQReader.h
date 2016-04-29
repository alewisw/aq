#ifndef IAQREADER_H
#define IAQREADER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQItem.h"




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
class IAQReader
{
public:

    // Constructor and destructor stubs for this interface.
    IAQReader(void) { };
    virtual ~IAQReader(void) { };

    // See AQ and AQReader classes for the definition of these functions.
    virtual bool retrieve(AQItem& item) = 0;
    virtual void release(AQItem& item) = 0;

};

// Template for implementing an IAQWriter.
template<typename T> class TAQReader : public IAQReader
{
public:
    TAQReader(T& aq) : m_aq(aq) { };
private:
    TAQReader<T>(const TAQReader<T>& other);
    TAQReader<T>& operator=(const TAQReader<T>& other);
public:
    virtual ~TAQReader(void) { };

    virtual bool retrieve(AQItem& item) { return m_aq.retrieve(item); }
    virtual void release(AQItem& item) { m_aq.release(item); }

private:

    T& m_aq;

};




#endif
//=============================== End of File ==================================
