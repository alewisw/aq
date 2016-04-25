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
//
// The AQWriterItem can be either accessed directly and written with, e.g., 
// memcpy() or alternatly it can be appended to use the AQWriter::append() 
// function.  The AQWriterItem and this function behave differently depending
// on the options of the queue:
//  - When the queue is OPTION_EXTENDABLE appending bytes to the item results
//    in the size of the item increasing, and new items being allocated as
//    required.
//  - When the queue does *NOT* have OPTION_EXTENDABLE the size of the item is
//    fixed at allocation time.  The item contains a hidden position that
//    starts at 0.  As data is appended the position increments.  The size
//    of the item never changes, nor is the position impacted by direct writes
//    into the memory buffer (e.g., via memcpy()).
class AQWriterItem : public AQItem
{
    // Fields are set directly from the MPAC queue objects.
    friend class AQWriter;

public:
    // Constructs a new item with no initial values.
    AQWriterItem(void) 
        : AQItem()
        , m_writer(NULL) 
        , m_accumulator(0)
    {
    }

    // Copy contructor - constructs this item as an exact copy of another item.
    AQWriterItem(const AQWriterItem& other)
        : AQItem(other)
        , m_writer(other.m_writer)
        , m_accumulator(other.m_accumulator)
    {
    }

    // Assigns the value of this item to exactly match another.
    AQWriterItem& operator=(const AQWriterItem& other)
    {
        if (this != &other)
        {
            AQItem::operator=(other);
            m_writer = other.m_writer;
            m_accumulator = other.m_accumulator;
        }
        return *this;
    };

    // Destructor for this item; does nothing.
    virtual ~AQWriterItem(void) { }

protected:

    // Returns a new instance of an AQItem matching the type of this
    // object.
    virtual AQItem *newInstance(void) const { return new AQWriterItem; }

private:

    // The writer that owns this item.
    AQWriter *m_writer;

    // The internal position variable used to track the append point for this 
    // item.  This contains the size of this item up to, and including,
    // this item in the chain.  If there are further items in the chain this 
    // *does not* include those items.
    size_t m_accumulator;

public:

    // Sets the link identifier for this item.
    void setLinkIdentifier(uint32_t lkid) { m_lkid = lkid; }

    // Provides a reference to one of the bytes in this item.  The address of
    // the memory can be taken in order to obtain the fixed size array for
    // reading.
    const unsigned char& operator[](size_t idx) const { return mem()[idx]; }

    // Provides a reference to one of the bytes in this item.  The address of
    // the memory can be taken in order to obtain the fixed size array for
    // reading.
    unsigned char& operator[](size_t idx) { return mem()[idx]; }

    // Writes 'memSize' bytes from 'mem' into this item at the current write 
    // position of this item.  If the write succeeds the write position is 
    // incremented by 'memSize'.  If the write fails then the write position
    // remains unchanged.
    //
    // If this writer item is not an uncommited claimed item from a queue then
    // a domain_error is thrown.
    //
    // If mem is NULL and memSize is non-zero then an invalid_argument exception
    // is thrown.
    //
    // In non-EXTENDABLE mode (that is OPTION_EXTENDABLE is not set) this throws
    // an out_of_range exception if the offse is greater than or equal to the size,
    // or a length_error exception if this would result in a write beyond the 
    // available size of the item.
    //
    // In EXTENDABLE mode new items are created to be able to contain the 
    // off + memSize bytes.  If they cannot be created then the write fails and
    // false is returned.
    //
    // If the data was written the write succeeds and true is returned.
    bool write(const void *mem, size_t memSize);

    // Writes 'memSize' bytes from 'mem' into this item at position 'off'.
    //
    // If this writer item is not an uncommited claimed item from a queue then
    // a domain_error is thrown.
    //
    // If mem is NULL and memSize is non-zero then an invalid_argument exception
    // is thrown.
    //
    // In non-EXTENDABLE mode (that is OPTION_EXTENDABLE is not set) this throws
    // an out_of_range exception if the offse is greater than or equal to the size,
    // or a length_error exception if this would result in a write beyond the 
    // available size of the item.
    //
    // In EXTENDABLE mode new items are created to be able to contain the 
    // off + memSize bytes.  If they cannot be created then the write fails and
    // false is returned.
    //
    // If the data was written the write succeeds and true is returned.
    bool write(size_t off, const void *mem, size_t memSize);

private:

    // Validates the offset and memory size for a normal write, returning this object.
    //
    // Updates the m_accumulator, if necessary, to the last written byte.
    AQWriterItem *writeAdvanceNormal(size_t off, size_t memSize);

    // Advances the extendable item sufficiently to support a write starting at 'off'
    // and ending at 'off' + 'memSize' - 1.  Returns the object where the writing
    // is to begin, updating 'off' to refer to that object.
    //
    // If the item could not be extended then NULL is returned.
    AQWriterItem *writeAdvanceExtendable(size_t& off, size_t memSize);

    // Extends the current item so that it can contain at least an additional memSize
    // bytes.
    bool extend(size_t memSize);

public:

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
