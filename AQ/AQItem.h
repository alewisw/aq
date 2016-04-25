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
     * Note that this does not create a new entry in the queue - it just copies the
     * reference to the existing item.  Regardless of how many copies are made
     * only a single call to AQWriter::commit() or AQReader::release() may be made.  
     * Accessing the memory of an item that has been committed or released elsewhere
     * results in undefined behavior.
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
     * Note that this does not create a new entry in the queue - it just copies the
     * reference to the existing item.  Regardless of how many copies are made
     * only a single call to AQWriter::commit() or AQReader::release() may be made.
     * Accessing the memory of an item that has been committed or released elsewhere
     * results in undefined behavior.
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
     * In all other cases this will return AQItem::QUEUE_IDENTIFIER_INVALID.
     * @returns The link identifier for this item.  If this item is not allocated
     * the returned value is undefined.
     */
    uint32_t linkIdentifier(void) const { return m_lkid; }

    /**
     * Determines if this item was committed to the queue with a call to 
     * AQWriter::commit().  There are a number of situations to consider when
     * inspecting this value:
     *  * For items claimed by calling AQWriter::claim() this always returns false. 
     *  * When an item is retrieved from AQReader::retrieve() this indicates whether 
     *    the item was actually successfully committed.  Items might not have been 
     *    commited if, for example, the process that claimed the item was terminated 
     *    before it was able to call AQWriter::commit().
     *  * When an item is retrieved from AQSnapshot::operator[] this indicates whether
     *    the item was committed at the time the snapshot taken.
     *
     * When processing a retrieved item that has isCommitted() as false the application
     * shoud either discard the item entirely, or use some application-specific method
     * to determine if the item is valid.
     *
     * Note that in the case of queues with the AQ::OPTION_EXTENDABLE format option
     * set if any one of the items was not committed then all the items in the linked
     * list are maked as not committed.
     *
     * @returns True if the item was committed to the queue or false if it was not 
     * committed.  If this item is not allocated the returned value is undefined.
     */
    bool isCommitted(void) const;

    /**
     * Determines if this item has been released from the queue with a call to
     * AQReader::release().  This can only occur when an item is retrieved from 
     * AQSnapshot::operator[] where the AQReader::release() had been called on that
     * particular item.
     *
     * @returns True if this item has been released through AQReader::release() and
     * false in all other cases.  If this item is not allocated the returned value 
     * is undefined.
     */
    bool isReleased(void) const;

    /**
     * Determies if the checksum for this item is valid when AQ::OPTION_CRC32 is
     * set for this queue.  When AQ::OPTION_CRC32 is not set then this always
     * return true.
     *
     * Note that in the case of queues with the AQ::OPTION_EXTENDABLE format option
     * set if any one of the items had an invalid checksum then all the items in the 
     * linked are maked as with invalid checksums.
     *
     * @returns False if AQ::OPTION_CRC32 is enabled and the checksum calculated 
     * did not match the one stored in the.  Returns true in all other cases 
     * except where this item is not allocated in which case the returned value is 
     * undefined.
     */
    bool isChecksumValid(void) const { return m_checksumValid; }

    /**
     * Obtains the total number of bytes that can be stored in this item.  This
     * only ever refers to this particular item - when AQ::OPTION_EXTENDABLE is
     * set each individual AQItem in the linked list has its own capacity value.
     *
     * @returns The capacity of this item in bytes.  If this item is not allocated
     * the returned value is undefined.
     */
    size_t capacity(void) const;

    /**
     * Obtains the total number of bytes that have been stored by the user into
     * this item.  This is the same as capacity() unless AQ::OPTION_EXTENDABLE
     * has been configured for this queue.  In that case the following rules
     * apply:
     *  * For each item in the linked list, except the last, size() exactly
     *    matches capacity().
     *  * For the last item in the linked list size() represents the highest
     *    byte that has been written into the item.
     *
     * @returns The size of this item in bytes.  If this item is not allocated
     * the returned value is undefined.
     */
    size_t size(void) const { return m_memSize; }

    /**
     * Obtains a reference to one of the bytes within this item.  This 
     * reference is only to be used for reading; writing the byte results
     * in undefined behavior.  The bytes in this item are allocated 
     * contiguously which means applications can do:
     * ~~~
     *     unsigned char *ptr = &item[0];
     * ~~~
     * In order to obtain a pointer to the underlying array.
     *
     * No bounds checking is performed on access via this operator.  As such the
     * caller must ensure that idx is in the range 0 to (size() - 1) inclusive. 
     * The effect of accessing a byte outside of this range, even through
     * the pointer taken in the example above, is undefined.
     *
     * @param idx The index of the byte to retreive.  Must be in the range of 0 to
     * (size() - 1).
     * @returns A read-only reference to the specified byte.  If this item is not allocated
     * or the provided index is outside the range of bytes in this item than the 
     * returned value is undefined.
     */
    const unsigned char& operator[](size_t idx) const { return m_mem[idx]; }
    
    /**
     * Obtains a pointer to the first item in the linked list of items.  The 
     * returned pointer is always identical to this unless AQ::OPTION_EXTENDABLE 
     * has been set for the queue.
     *
     * @returns The first item.  This is never NULL.
     */
    const AQItem *first(void) const { return m_first; }

    /**
     * Obtains a pointer to the last item in the linked list of items.  The 
     * returned pointer is always identical to this unless AQ::OPTION_EXTENDABLE 
     * has been set for the queue.
     *
     * @returns The last item.  This is never NULL.
     */
    const AQItem *last(void) const { return m_first->m_prev; }

    /**
     * Obtains a pointer to the next item in the linked list of items.  If this
     * is the last item in the list then NULL is returned.
     *
     * @returns The next item or NULL if there are no further items.  NULL is always
     * returned unless AQ::OPTION_EXTENDABLE is set.
     */
    const AQItem *next(void) const { return m_next; }

    /**
    * Obtains a pointer to the previous item in the linked list of items.  If this
    * is the first item in the list then NULL is returned.
    *
    * @returns The previous item or NULL if there are no further items.  NULL is always
    * returned unless AQ::OPTION_EXTENDABLE is set.
    */
    const AQItem *prev(void) const { return m_first == this ? NULL : m_prev; }

};




#endif
//=============================== End of File ==================================
