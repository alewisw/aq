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

/**
 * Encapsulates the memory for an item that resides with a Multi-Producer 
 * Concurrent Allocating Queue.
 */
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
    /**
     * Constructs a new item with no initial allocation.
     */
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

    /**
     * Constructs a new item so that it is an exact copy of another item.
     *
     * @param other The other item to copy.
     */
    AQItem(const AQItem& other) 
        : m_first(this)
        , m_prev(this)
        , m_next(NULL)
    {
        cloneFrom(other);
    }

    /**
     * Assigns this item so that it is an exact copy of another item.
     *
     * @param other The other item to copy.
     */
    AQItem& operator=(const AQItem& other)
    {
        if (this != &other)
        {
            cloneFrom(other);
        }
        return *this;
    }

    /**
     * Destroys this item.  This will not take any action on the underlying
     * queue such as calling AQWriter::commit() or AQReader::release().  It
     * is entirely up to the application to ensure that each item that must
     * be committed or released has the appropriate action taken.
     */
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

    // The control field data for this item.
    uint32_t m_ctrl;

    // The queue identifier for this item.
    uint32_t m_quid;

    // Set to true if the checksum for this item was valid.
    bool m_checksumValid;

protected:

    // The memory in this item.
    unsigned char *m_mem;

    // The size of the memory region.
    size_t m_memSize;

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

    /**
     * The AQItem::queueIdentifier() will always return a value that fits
     * within this bitmask.  The bitmask has the top 3 bits clear, thus
     * all queue identifiers fall into the range 0 - AQ::QUEUE_IDENTIFIER_MASK.
     */
    static const uint32_t QUEUE_IDENTIFIER_MASK = 0x1FFFFFFF;

    /**
     * The AQItem::queueIdentifier() will always have the bits in this mask
     * set to 0.  This is the inverse of AQ::QUEUE_IDENTIFIER_MASK.
    */
    static const uint32_t QUEUE_IDENTIFIER_USER_MASK = 0xE0000000;

    /**
     * The first unused bit in the AQItem::queueIdentifier().  The queue
     * identifier will never set this bit or an higher bit.
     */
    static const uint32_t QUEUE_IDENTIFIER_USER_BIT = 29;

    /**
     * If a queue identifier is not known or has not been allocated this
     * value will be stored in AQItem::queueIdentifier() or 
     * AQItem::linkIdentifier().
     */
    static const uint32_t QUEUE_IDENTIFIER_INVALID = 0xFFFFFFFF;
    
private:
    
    // When set this bit indicates that this is the first AQ item of an XAQ item.
    static const uint32_t LINK_IDENTIFIER_FIRST = 1 << AQItem::QUEUE_IDENTIFIER_USER_BIT;

    // When set this bit indicates that this is the last  AQ item of an XAQ item.
    // The last item lists the total size of the XAQ item.
    static const uint32_t LINK_IDENTIFIER_LAST = 1 << (AQItem::QUEUE_IDENTIFIER_USER_BIT + 1);
    
public:

    /**
     * Clears the content of this item so that it is no longer allocated.  
     * AQItem::isAllocated() now returns false.  When this item is the first
     * entry in a linked list this has the effect of deleting all the other
     * links in the list such that AQItem::next() now returns NULL.  When this
     * item is part of a linked list, but no the first item, it just clears 
     * the state of this item without impacting the list itself.
     */
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

    /**
     * Determins if this item has been allocated to part of a queue.  This only
     * returns true if there is a backing item within the queue that has provided
     * memory that can be read (and written in the case of AQWriterItem).
     * @returns True if this item has been allocated, false if it has not been 
     * allocated.  Note that after AQWriter::commit() and AQReader::release() are
     * called the items passed to those functions are automatically deallocated.
     */
    bool isAllocated(void) const { return m_mem != NULL; }

    /**
     * Obtains the unique queue identifier for this item.  The queue identifier
     * is a a number in the range 0 - AQItem::QUEUE_IDENTIFIER_MASK that
     * uniquely identifies this item within the current queue state.  So long as
     * this item is not released from the queue, it is guarnteed that no
     * other item will have the same queue identifier.
     * @returns The queue identifier for this item.  If this item is not allocated
     * the returned value is undefined.
     */
    uint32_t queueIdentifier(void) const { return m_quid; }

    /**
     * Obtains the link identifier for this item.  When the queue is formatted
     * with AQ::OPTION_EXTENDABLE this is used internally to track the linked
     * list of items.  In this case applications should not inspect nor rely
     * on this value having any particular meaning.
     *
     * When AQ::OPTION_EXTENDABLE is not set, but AQ::OPTION_LINK_IDENTIFIER
     * has been set then applications can set the link identifier when writing 
     * an item using AQWriterItem::setLinkIdentifier() and have the same value
     * make available through this function when the item is read.
     *
     * In all other cases this will return AQ::QUEUE_IDENTIFIER_INVALID.
     * @returns The link identifier for this item.  If this item is not allocated
     * the returned value is undefined.
     */
    uint32_t linkIdentifier(void) const { return m_lkid; }

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
