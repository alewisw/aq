#ifndef AQWRITERITEM_H
#define AQWRITERITEM_H
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

// Encapsulates an item is being written to a Multi-Producer Allocating 
// Concurrent queue.
class AQWriterItem : public AQItem
{
    // Fields are set directly from the MPAC queue objects.
    friend class AQWriter;

public:
    // Constructs a new item with no initial values.
    AQWriterItem(void) { };

    // Copy contructor - constructs this item as an exact copy of another item.
    AQWriterItem(const AQWriterItem& other)
        : AQItem(other)
    {
    };

    // Assigns the value of this item to exactly match another.
    AQWriterItem& operator=(const AQWriterItem& other)
    {
        if (this != &other)
        {
            AQItem::operator=(other);
        }
        return *this;
    };

    // Destructor for this item; does nothing.
    virtual ~AQWriterItem(void) { };

protected:

    // Returns a new instance of an AQItem matching the type of this
    // object.
    virtual AQItem *newInstance(void) const { return new AQWriterItem; }

public:

    // Sets the link identifier for this item.
    void setLinkIdentifier(uint32_t lkid) { m_lkid = lkid; }

    // Provides a reference to one of the bytes in this item.  The address of
    // the memory can be taken in order to obtain the fixed size array for
    // reading.
    const unsigned char& operator[](size_t idx) const { return mem()[idx]; }

    // Provides a writeable reference to one of the bytes in this item.  The 
    // address of the memory can be taken in order to obtain the fixed size 
    // array for writing.
    unsigned char& operator[](size_t idx) { return mem()[idx]; }

    // Linked list iterators for the AQWriterItem.
    AQWriterItem *first(void) { return (AQWriterItem *)AQItem::first(); }
    AQWriterItem *last(void) { return (AQWriterItem *)AQItem::last(); }
    AQWriterItem *next(void) { return (AQWriterItem *)AQItem::next(); }
    AQWriterItem *prev(void) { return (AQWriterItem *)AQItem::prev(); }

    const AQWriterItem *first(void) const { return (const AQWriterItem *)AQItem::first(); }
    const AQWriterItem *last(void) const { return (const AQWriterItem *)AQItem::last(); }
    const AQWriterItem *next(void) const { return (const AQWriterItem *)AQItem::next(); }
    const AQWriterItem *prev(void) const { return (const AQWriterItem *)AQItem::prev(); }

};




#endif
//=============================== End of File ==================================
