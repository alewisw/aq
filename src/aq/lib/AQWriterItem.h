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

#include <stdarg.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQWriter;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Encapsulates an item is being written to a Multi-Producer Concurrent 
 * Allocating Queue.
 *
 * The AQWriterItem can be either accessed directly and written with, e.g., 
 * memcpy() or alternatly it can be written to using the 
 * write(const void *, size_t, InsufficientSpaceAction) or similar functions.  When the 
 * AQ::OPTION_EXTENDABLE option has been set the write(const void *, size_t, InsufficientSpaceAction)
 * functions are the main mechanism for extending the length of the item
 * by appending new items to the end of the linked list.
 */
class AQWriterItem : public AQItem
{
    // Fields are set directly from the MPAC queue objects.
    friend class AQWriter;

public:

    /**
     * Constructs a new item with no initial allocation.
     */
    AQWriterItem(void)
        : AQItem()
        , m_writer(NULL) 
        , m_accumulator(0)
    {
    }

    /**
     * Constructs a new item so that it is an exact copy of another item.  Note
     * that this does not create a new entry in the queue - it just copies the
     * references from the other item.  Regardless of how many copies are made
     * only a single call to AQWriter::commit() may be made.  Accessing the
     * memory of an item that has been committed elsewhere results in undefined
     * behavior.
     *
     * @param other The other item to copy.
     */
    AQWriterItem(const AQWriterItem& other)
        : AQItem(other)
        , m_writer(other.m_writer)
        , m_accumulator(other.m_accumulator)
    {
    }

    /**
     * Assigns this item so that it is an exact copy of another item. Note
     * that this does not create a new entry in the queue - it just copies the
     * references from the other item.  Regardless of how many copies are made
     * only a single call to AQWriter::commit() may be made.  Accessing the
     * memory of an item that has been committed elsewhere results in undefined
     * behavior.
     *
     * @param other The other item to copy.
     * @returns A reference to this object.
     */
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

    /**
     * Destroys this item.  This will not take any action on the underlying
     * queue such as calling AQWriter::commit() or AQReader::release().  It
     * is entirely up to the application to ensure that each item that must
     * be committed or released has the appropriate action taken.
     */
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

    /**
     * Sets the link identifier for this item.  Setting the link identifier
     * only makes sense when AQ::OPTION_LINK_IDENTIFIER has been set on the
     * queue and AQ::OPTION_EXTENDABLE is not set.  In all other situations
     * the link identifier set here will not propogate through to the item
     * reader via AQReader::retrieve().  See AQItem::linkIdentifier() for
     * more information on why this is the case.
     *
     * @param lkid The link identifier value to set for this item.
     */
    void setLinkIdentifier(uint32_t lkid) { m_lkid = lkid; }

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
     * or the provided index is outside the range of bytes in this item then the
     * returned value is undefined.
     */
    const unsigned char& operator[](size_t idx) const { return mem()[idx]; }

    /**
     * Obtains a reference to one of the bytes within this item.  This
     * reference can be used for both reading and writing.  The bytes in this 
     * item are allocated contiguously which means applications can do:
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
     * @returns A read-write reference to the specified byte.  If this item is not allocated
     * or the provided index is outside the range of bytes in this item then the
     * returned value is undefined.
     */
    unsigned char& operator[](size_t idx) { return mem()[idx]; }

    /**
     * Defines the possible actions to be taken when writing
     * to a queue with insufficient space to contain the entire buffer.
     */
    enum InsufficientSpaceAction
    {
        /**
         * When there is not enough space available to contain the entire
         * buffer, and the item cannot be extended to make the space available,
         * don't write anything into the item.  The item remains unchanged.
         */
        WRITE_NONE,

        /**
         * Write as much as possible into the item, effectivly truncating
         * the buffer.
         */
        WRITE_PARTIAL
    };

    /**
     * Writes data into this item at its current write position.  The write
     * position differs depending on whether AQ::OPTION_EXTENDABLE has been
     * configured:
     *  * When AQ::OPTION_EXTENDABLE is not set then the item internaly tracks
     *    the highest index where no write operation has been performed.  The
     *    write occurs at that index.
     *  * When AQ::OPTION_EXTENDABLE is set then the write occurs at the very
     *    end of the item, essentially appending and possibly extending the
     *    size of the item by adding to the linked list.
     * If the write succeeds then the write position is incremented by the 
     * number of bytes written.  If this fails (an exception is thrown or more
     * items could not be allocated) no changes are made to this item.
     *
     * @param mem The buffer that contains the memory to write into this item.
     * @param memSize The number of bytes to write into this item.
     * @param insufficientSpaceAction The action to take when the item cannot store
     * the entire buffer (`memSize` bytes).
     * @returns The actual number of bytes written into the buffer.  The possible
     * return values depend on the value of the `insufficientSpaceAction` parameter:
     *  - When `insufficientSpaceAction` is InsufficientSpaceAction::WRITE_NONE then either 0
     *    or `memSize` is returned.  When 0 is returned no data was written,
     *    when `memSize` is returned the entire buffer was written.
     *  - When `insufficientSpaceAction` is InsufficientSpaceAction::WRITE_PARTIAL the return
     *    value is between 0 and `memSize` inclusive and is the actual number of bytes
     *    written from the buffer.
     * @throws std::domain_error If this item was not populated by a successful 
     * call to AQWriter::claim() or if it has been committed with a call to 
     * AQWriter::commit().
     * @throws std::invalid_argument If the mem argument was NULL and memSize was
     * any value other than 0.
     */
    size_t write(const void *mem, size_t memSize, 
        InsufficientSpaceAction insufficientSpaceAction = WRITE_NONE)
    {
        return write(currentOffset(), mem, memSize, insufficientSpaceAction);
    }

    /**
    * Writes data into this item at a selected position.  If the write succeeds 
    * then the write position as used by write(const void *, size_t, InsufficientSpaceAction) is set to 
    * the maximum of its current value and off + memSize.  If the write fails
    * (an exception is thrown or more items could not be allocated) no changes
    * are made to this item.
    *
    * @param off The offset from this item where the write is to be performed.
    * For AQ::OPTION_EXTENDABLE items this offset may be larger than the size()
    * of this item in which case it finds the item that contains that offset
    * and starts the write there.
    * @param mem The buffer that contains the memory to write into this item.
    * @param memSize The number of bytes to write into this item.
    * @param insufficientSpaceAction The action to take when the item cannot store
    * the entire buffer (`memSize` bytes).
    * @returns The actual number of bytes written into the buffer.  The possible
    * return values depend on the value of the `insufficientSpaceAction` parameter:
    *  - When `insufficientSpaceAction` is InsufficientSpaceAction::WRITE_NONE then either 0
    *    or `memSize` is returned.  When 0 is returned no data was written,
    *    when `memSize` is returned the entire buffer was written.
    *  - When `insufficientSpaceAction` is InsufficientSpaceAction::WRITE_PARTIAL the return
    *    value is between 0 and `memSize` inclusive and is the actual number of bytes
    *    written from the buffer.
    * @throws std::domain_error If this item was not populated by a successful
    * call to AQWriter::claim() or if it has been committed with a call to
    * AQWriter::commit().
    * @throws std::invalid_argument If the mem argument was NULL and memSize was
    * any value other than 0.
    * @throws std::out_of_range If the queue does not have the AQ::OPTION_EXTENDABLE
    * option set and the offest is at the end, or beyond the end, of the item.
    * @throws std::length_error If the queue does not have the AQ::OPTION_EXTENDABLE
    * option set and there is not enough space left in the queue to store the
    * requested number of bytes at the given offset.
    */
    size_t write(size_t off, const void *mem, size_t memSize,
        InsufficientSpaceAction insufficientSpaceAction = WRITE_NONE);

private:

    // Returns the current write offset for this item.
    size_t currentOffset(void) const;

    // Depending on whether a normal write or extendable write is required this
    // simply defers to writeAdvanceNormal() or writeAdvanceExtendable().
    AQWriterItem *writeAdvance(size_t& off, size_t& memSize,
        InsufficientSpaceAction insufficientSpaceAction);

    // Validates the offset and memory size for a normal write, returning this object.
    //
    // Updates the 'memSize' to reflect the actual number of writeable bytes.
    //
    // Updates the m_accumulator, if necessary, to the last written byte.
    AQWriterItem *writeAdvanceNormal(size_t off, size_t& memSize,
        InsufficientSpaceAction insufficientSpaceAction);

    // Advances the extendable item sufficiently to support a write starting at 'off'
    // and ending at 'off' + 'memSize' - 1.  Returns the object where the writing
    // is to begin, updating 'off' to refer to that object and 'memSize' to the actual
    // number of writeable bytes.
    //
    // If the item could not be extended then NULL is returned.
    AQWriterItem *writeAdvanceExtendable(size_t& off, size_t& memSize,
        InsufficientSpaceAction insufficientSpaceAction);

    // Extends the current item so that it can contain at least an additional memSize
    // bytes.
    bool extend(size_t memSize, InsufficientSpaceAction insufficientSpaceAction);

public:

    /**
     * Prints a formatted string into this item at its current write position.
     * The write position is incremented by the number of bytes actually written.
     * If there is not enough space in the item then the print is truncated
     * to the available space.
     *
     * The string written into the item is not nul-terminated.
     *
     * @param fmt The printf-style formatting string.
     * @param ... The printf-style formatting arguments.
     * @returns There are two possibilities:
     *  - The return value is non-negative (>= 0).  In this case the string was
     *    fully written into the item and the total number of bytes consumed is
     *    returned.
     *  - The return value is negative (< 0).  In this case it was not possible
     *    to fully write the string into the item, nor was it possible to allocate
     *    further items to the list if the queue is in AQ::OPTION_EXTENDABLE mode.
     *    Only part of the string has been written.  The length of the partially 
     *    written string can be obtained by applying the binary 1's-complement 
     *    operator (~) to the returned value.
     * @throws std::domain_error If this item was not populated by a successful
     * call to AQWriter::claim() or if it has been committed with a call to
     * AQWriter::commit().
     * @throws std::invalid_argument If the fmt argument was NULL.
     */
    int printf(const char *fmt, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 3)))
#endif
    {
        va_list argp;
        va_start(argp, fmt);
        int res = vprintf(fmt, argp);
        va_end(argp);
        return res;
    }

    /**
     * Prints a formatted string into this item at its current write position.
     * The write position is incremented by the number of bytes actually written.
     * If there is not enough space in the item then the print is truncated
     * to the available space.
     *
     * The string written into the item is not nul-terminated.
     *
     * @param fmt The printf-style formatting string.
     * @param argp The printf-style formatting arguments.
     * @returns There are two possibilities:
     *  - The return value is non-negative (>= 0).  In this case the string was
     *    fully written into the item and the total number of bytes consumed is
     *    returned.
     *  - The return value is negative (< 0).  In this case it was not possible
     *    to fully write the string into the item, nor was it possible to allocate
     *    further items to the list if the queue is in AQ::OPTION_EXTENDABLE mode.
     *    Only part of the string has been written.  The length of the partially
     *    written string can be obtained by applying the binary 1's-complement
     *    operator (~) to the returned value.
     * @throws std::domain_error If this item was not populated by a successful
     * call to AQWriter::claim() or if it has been committed with a call to
     * AQWriter::commit().
     * @throws std::invalid_argument If the fmt argument was NULL.
     */
    int vprintf(const char *fmt, va_list argp)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 0)))
#endif
    {
        return vprintf(currentOffset(), fmt, argp);
    }

    /**
     * Prints a formatted string into this item at a selected position.  If
     * the write succeeds then the write position as used by
     * write(const void *, size_t, InsufficientSpaceAction) is set to the maximum of its current value
     * and off + memSize.  If there is not enough space in the item then the
     * print is truncated to the available space.
     *
     * The string written into the item is not nul-terminated.
     *
     * @param off The offset from this item where the write is to be performed.
     * For AQ::OPTION_EXTENDABLE items this offset may be larger than the size()
     * of this item in which case it finds the item that contains that offset
     * and starts the write there.
     * @param fmt The printf-style formatting string.
     * @param ... The printf-style formatting arguments.
     * @returns There are two possibilities:
     *  - The return value is non-negative (>= 0).  In this case the string was
     *    fully written into the item and the total number of bytes consumed is
     *    returned.
     *  - The return value is negative (< 0).  In this case it was not possible
     *    to fully write the string into the item, nor was it possible to allocate
     *    further items to the list if the queue is in AQ::OPTION_EXTENDABLE mode.
     *    Only part of the string has been written.  The length of the partially
     *    written string can be obtained by applying the binary 1's-complement
     *    operator (~) to the returned value.
     * @throws std::domain_error If this item was not populated by a successful
     * call to AQWriter::claim() or if it has been committed with a call to
     * AQWriter::commit().
     * @throws std::invalid_argument If the fmt argument was NULL.
     */
    int printf(size_t off, const char *fmt, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 4)))
#endif
    {
        va_list argp;
        va_start(argp, fmt);
        int res = vprintf(off, fmt, argp);
        va_end(argp);
        return res;
    }

    /**
    * Prints a formatted string into this item at a selected position.  If
    * the write succeeds then the write position as used by
    * write(const void *, size_t, InsufficientSpaceAction) is set to the maximum of its current value
    * and off + memSize.  If there is not enough space in the item then the
    * print is truncated to the available space.
    *
    * @param off The offset from this item where the write is to be performed.
    * For AQ::OPTION_EXTENDABLE items this offset may be larger than the size()
    * of this item in which case it finds the item that contains that offset
    * and starts the write there.
    * @param fmt The printf-style formatting string.
    * @param argp The printf-style formatting arguments.
    * @returns There are two possibilities:
    *  - The return value is non-negative (>= 0).  In this case the string was
    *    fully written into the item and the total number of bytes consumed is
    *    returned.
    *  - The return value is negative (< 0).  In this case it was not possible
    *    to fully write the string into the item, nor was it possible to allocate
    *    further items to the list if the queue is in AQ::OPTION_EXTENDABLE mode.
    *    Only part of the string has been written.  The length of the partially
    *    written string can be obtained by applying the binary 1's-complement
    *    operator (~) to the returned value.
    * @throws std::domain_error If this item was not populated by a successful
    * call to AQWriter::claim() or if it has been committed with a call to
    * AQWriter::commit().
    * @throws std::invalid_argument If the fmt argument was NULL.
    */
    int vprintf(size_t off, const char *fmt, va_list argp)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 0)))
#endif
        ;

public:

    /**
     * Obtains a pointer to the first item in the linked list of items.  The 
     * returned pointer is always identical to this unless AQ::OPTION_EXTENDABLE 
     * has been set for the queue.
     *
     * @returns The first item.  This is never NULL.
     */
    AQWriterItem *first(void) { return (AQWriterItem *)AQItem::first(); }

    /**
     * Obtains a pointer to the last item in the linked list of items.  The 
     * returned pointer is always identical to this unless AQ::OPTION_EXTENDABLE 
     * has been set for the queue.
     *
     * @returns The last item.  This is never NULL.
     */
    AQWriterItem *last(void) { return (AQWriterItem *)AQItem::last(); }

    /**
     * Obtains a pointer to the next item in the linked list of items.  If this
     * is the last item in the list then NULL is returned.
     *
     * @returns The next item or NULL if there are no further items.  NULL is always
     * returned unless AQ::OPTION_EXTENDABLE is set.
     */
    AQWriterItem *next(void) { return (AQWriterItem *)AQItem::next(); }

    /**
     * Obtains a pointer to the previous item in the linked list of items.  If this
     * is the first item in the list then NULL is returned.
     *
     * @returns The previous item or NULL if there are no further items.  NULL is always
     * returned unless AQ::OPTION_EXTENDABLE is set.
     */
    AQWriterItem *prev(void) { return (AQWriterItem *)AQItem::prev(); }

    /**
     * Obtains a read-only pointer to the first item in the linked list of items.  The
     * returned pointer is always identical to this unless AQ::OPTION_EXTENDABLE
     * has been set for the queue.
     *
     * @returns The first item.  This is never NULL.
     */
    const AQWriterItem *first(void) const { return (const AQWriterItem *)AQItem::first(); }

    /**
     * Obtains a read-only pointer to the last item in the linked list of items.  The
     * returned pointer is always identical to this unless AQ::OPTION_EXTENDABLE
     * has been set for the queue.
     *
     * @returns The last item.  This is never NULL.
     */
    const AQWriterItem *last(void) const { return (const AQWriterItem *)AQItem::last(); }
    
    /**
     * Obtains a read-only pointer to the next item in the linked list of items.  If this
     * is the last item in the list then NULL is returned.
     *
     * @returns The next item or NULL if there are no further items.  NULL is always
     * returned unless AQ::OPTION_EXTENDABLE is set.
     */
    const AQWriterItem *next(void) const { return (const AQWriterItem *)AQItem::next(); }
    
    /**
     * Obtains a read-only pointer to the previous item in the linked list of items.  If this
     * is the first item in the list then NULL is returned.
     *
     * @returns The previous item or NULL if there are no further items.  NULL is always
     * returned unless AQ::OPTION_EXTENDABLE is set.
     */
    const AQWriterItem *prev(void) const { return (const AQWriterItem *)AQItem::prev(); }

};




#endif
//=============================== End of File ==================================
