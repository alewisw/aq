#ifndef AQITEM_H
#define AQITEM_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <cstdint>
#include <cstdlib>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
namespace aq
{
    class TraceBuffer;
    class LinkedItemProcessor;
}
class AQReferenceBase;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates an item that resides with a Multi-Producer Allocating Concurrent
// queue.
class AQItem
{
    // Fields are set directly from the MPAC objects.
    friend class AQReader;
    friend class AQWriter;
    friend class XAQReader;
    friend class XAQWriter;
    friend class AQSnapshot;

    // The trace buffer inspects the content of this item to generate trace data.
    friend class aq::TraceBuffer;
    friend class aq::LinkedItemProcessor;

    // Fields are directly maniulated from the unit test framework.
    friend class AQTest;

    // Used internally for testing.
    friend class AQStrawManBase;

public:
    // Constructs a new item with no initial values.
    AQItem(void) 
        : m_first(this)
        , m_prev(this)
        , m_next(NULL)
        , m_mem(NULL)
        , m_memSize(0)
        , m_ctrl(0)
        , m_checksumValid(false)
        , m_quid(QUEUE_IDENTIFIER_INVALID)
        , m_lkid(QUEUE_IDENTIFIER_INVALID)
    {
    }

    // Copy contructor - constructs this item as an exact copy of another item.
    AQItem(const AQItem& other) 
        : m_first(this)
        , m_prev(this)
        , m_next(NULL)
    {
        cloneFrom(other);
    }

    // Assigns the value of this item to exactly match another.
    AQItem& operator=(const AQItem& other)
    {
        if (this != &other)
        {
            cloneFrom(other);
        }
        return *this;
    }

    // Destructor for this item; does nothing.
    virtual ~AQItem(void)
    {
        clearList();
    }

protected:

    // Returns a new instance of an AQItem matching the type of this
    // object.
    virtual AQItem *newInstance(void) const { return new AQItem; }

private:

    // Clones this item from another item 'other'.
    void cloneFrom(const AQItem& other);

    // Clears the linked list if this is the first item in the list.
    void clearList();

    // The memory in this item.
    unsigned char *m_mem;

    // The size of the memory region.
    size_t m_memSize;

    // The control field data for this item.
    uint32_t m_ctrl;

    // The queue identifier for this item.
    uint32_t m_quid;

    // Set to true if the checksum for this item was valid.
    bool m_checksumValid;

protected:

    // When used in a linked list of items this points to the first item
    // in the list.
    AQItem *m_first;

    // When used in a linked list of items these are the next item and
    // previous item pointers.  The first item (given by m_first) has
    // m_prev set to point to the last item in the queue.  The last
    // item in the queue has m_next set to NULL.
    AQItem *m_next;
    AQItem *m_prev;

    // The link identifier for this item.
    uint32_t m_lkid;

    // Obtains the pointer to the underlying memory.
    unsigned char *mem(void) const { return m_mem; }

    // Obtains the control word value for this item.
    uint32_t ctrl(void) const { return m_ctrl; }

public:

    // The mask for all queue identifiers; the top 3 bits are always
    // cleared.
    static const uint32_t QUEUE_IDENTIFIER_MASK = 0x1FFFFFFF;

    // The mask for the user identifiabe bits of the queue identifier.
    static const uint32_t QUEUE_IDENTIFIER_USER_MASK = 0xE0000000;

    // The bottom bit for the queue identifier free bits.
    static const uint32_t QUEUE_IDENTIFIER_USER_BIT = 29;

    // The value indicating that a queue identifier is invalid or has
    // not been set.
    static const uint32_t QUEUE_IDENTIFIER_INVALID = 0xFFFFFFFF;
    
private:
    
    // When set this bit indicates that this is the first AQ item of an XAQ item.
    static const uint32_t LINK_IDENTIFIER_FIRST = 1 << AQItem::QUEUE_IDENTIFIER_USER_BIT;

    // When set this bit indicates that this is the last  AQ item of an XAQ item.
    // The last item lists the total size of the XAQ item.
    static const uint32_t LINK_IDENTIFIER_LAST = 1 << (AQItem::QUEUE_IDENTIFIER_USER_BIT + 1);
    
public:

    // Clears the content of this item back to its defaults.
    void clear(void)
    {
        clearList();
        m_mem = NULL;
        m_memSize = 0;
        m_ctrl = 0;
        m_checksumValid = false;
        m_quid = QUEUE_IDENTIFIER_INVALID;
        m_lkid = QUEUE_IDENTIFIER_INVALID;
    }

    // Returns the queue unique identifier for this item.
    uint32_t queueIdentifier(void) const { return m_quid; }

    // Returns the link identifier for this item.
    uint32_t linkIdentifier(void) const { return m_lkid; }

    // Returns true if and only if this item exists in the sense that backing
    // memory has been allocated to this item.
    bool isAllocated(void) const { return m_mem != NULL; }

    // Returns true if this item was committed to the queue; if this is false then
    // the item should be considered potentially incomplete.
    bool isCommitted(void) const;

    // Returns true if this item was released at the time the snapshot was taken.
    bool isReleased(void) const;

    // Returns true if this item's checksum was valid (i.e., the stored value
    // matched the calculated value).  If there are no checksums enabled then
    // this always returns 'true'.
    bool isChecksumValid(void) const { return m_checksumValid; }

    // Gets the size, in bytes, of this item.
    size_t size(void) const { return m_memSize; }

    // Gets the capacity of this item - that is number of bytes that can be stored
    // in this item.
    size_t capacity(void) const;

    // Provides a reference to one of the bytes in this item.  The address of
    // the memory can be taken in order to obtain the fixed size array for
    // reading.
    const unsigned char& operator[](size_t idx) const { return m_mem[idx]; }
    
    // Linked list iterators for the AQItem.
    const AQItem *first(void) const { return m_first; }
    const AQItem *last(void) const { return m_first->m_prev; }
    const AQItem *next(void) const { return m_next; }
    const AQItem *prev(void) const { return m_first == this ? NULL : m_prev; }
};




#endif
//=============================== End of File ==================================
