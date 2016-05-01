#ifndef AQWRITER_H
#define AQWRITER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQ.h"




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQWriterItem;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Implements the writer side of a Multi-Producer Concurrent Allocating Queue.
 * Given a single memory region any number of writers in any number of threads
 * may write into the queue at the same time.  The claim() and commit() 
 * functions are thread-safe with respect to both reading and writing the queue
 * with one exception.  If the queue is concurrently formatted by the reader
 * then the behavior is undefined.
 */
class AQWriter : public AQ
{
public:

    /**
     * Constructs a queue writer object that uses the passed shared memory region mem
     * of total size memSize bytes.  This does not read or write the memory -
     * it just sets up the internal pointers and references.
     *
     * Before the queue can be accesed it must be formatted with AQReader::format().
     *
     * @param mem The memory address where the queue is stored.
     * @param memSize The total size of the memory region where the queue is stored.
     */
    AQWriter(void *mem, size_t memSize);

    // As above with the addition of a tracing buffer that holds all queue access logs.
    // This is only used in the unit and stress tests to track queue accesses and help
    // debug issues.
    AQWriter(void *mem, size_t memSize, aq::TraceBuffer *trace);

    /**
     * Constructs this queue writer such that it references exactly the same underlying
     * memory range as another queue writer.
     *
     * @param other The other queue writer whose memory range is to be used by this queue.
     */
    AQWriter(const AQWriter& other);

    /**
    * Assigns the underlying memory range of this queue writer to exactly match the shared
    * memory range of another queue writer.
    *
    * @param other The other queue writer whose memory range is to be used by this queue.
    * @return A reference to this object.
    */
    AQWriter& operator=(const AQWriter& other);

    /**
     * Destroys this queue writer.  Any outstanding AQWriterItem objects otained by
     * calling AQWriter::claim() but not yet committed through AQWriter::commit() 
     * may no longer be accessed; accessing these objects results in undefined
     * behavior.
     *
     * The underlying memory of the queue is not impacted by this operation.
     */
    virtual ~AQWriter(void);

    /**
     * Obtains a reference to a memory address that changes whenever space is made
     * available in the queue for further write operations.  This can be used as
     * a cheap method of polling for 'space available'.
     *
     * The main purpose of this is to implement efficient polling:
     * ~~~
     * void pollClaim(AQWriter& writer, AQWriterItem& item, size_t memSize)
     * {
     *     const volatile uint32_t& freeCounter = writer.freeCounter();
     *     uint32_t count = freeCounter;
     *     while (!writer.claim(item, memSize))
     *     {
     *         while (count == freeCounter)
     *         {
     *             ... perform some other processing, sleep, etc ...
     *         }
     *         count = freeCounter;
     *     }
     * }
     * ~~~
     *
     * @returns A reference to an integer whose value changes whenever space is
     * made available in the queue.
     */
    const volatile uint32_t& freeCounter(void) const;

private:

    // Converts the passed memory size as provided by the user to a capacity
    // as used internally by the XAQ.
    size_t sizeToCapacity(size_t size) const;

public:

    /**
     * Specifies the rule to use when claiming memory.
     */
    enum ClaimSizeRule
    {
        /**
         * Claim an item consisting of at least the specified memory size.
         * The amount claimed will be at least the amount specified.
         */
        CLAIM_EXACT,

        /**
         * Claim an item consisting of at most the specified memory size.
         * The amount claimed can be less then the specified amount.
         */
        CLAIM_AT_MOST,
    };

    /**
     * Claims an item to be stored in the queue.  The item consists of a 
     * memory region of size memSize bytes that is written into by the
     * caller.  
     *
     * The value stored in the returned memory becomes available to the 
     * reader once the returned item is passed to commit().  It must be 
     * committed within the commit timeout period of the queue or it
     * is automatically reclaimed.  Writes to the item after the commit
     * timeout has expired can result in data corruption for other items.
     *
     * @param item The writer item, supplied by the caller, which is
     * updated with the allocated memory and related parameters.
     * @param memSize The amount of memory that is required
     * for this allocation.  This field is treated slightly differently
     * depending on the formatting options for the queue:
     *  - If AQ::OPTION_EXTENDABLE is not set this represents exactly
     *    the number of bytes occupied by this item.  It cannot be 
     *    subsequetly changed or altered in any way.  AQWriterItem::size() 
     *    and AQWriterItem::capacity() will always return this value.
     *  - If AQ::OPTION_EXTENDABLE is set then this is the minimum size
     *    for the first item in the linked-list of extendable items.
     *    The AQWriteItem::size() will initially be set to 0, while
     *    the AQWriteItem::capacity() will be at least memSize although
     *    it may be more.
     * @param claimSizeRule Indicates how the `memSize` field is interpreted.
     * When set to ClaimSizeRule::CLAIM_EXACT the allocated memory is at least
     * that required by `memSize` (dependent on AQ::OPTION_EXTENDABLE).  When
     * set to ClaimSizeRule::CLAIM_AT_MOST the allocated memory can be less 
     * than the specified amount.  It will never be less if there was enough
     * space in the queue to satisfy the request.
     * @returns If the region could be claimed then true is returned and 
     * item updated to contain the valid memory.  If the item could not be
     * claimed then false is returned and item is marked as not allocated 
     * (AQItem::isAllocated() returns false).
     * @throws std::invalid_argument When the memSize parameter is greater than
     * the maximum allocation size for an AQ (1 MB) or when the queue is not
     * extendable (AQ::OPTION_EXTENDABLE is not set) and the memSize 
     * parameter was 0.
     * @throws AQUnformattedException When the queue is not formatted.
     */
    bool claim(AQWriterItem& item, size_t memSize, 
        ClaimSizeRule claimSizeRule = ClaimSizeRule::CLAIM_EXACT);

    /**
     * Commits an item previously obtained via a claim() call to
     * the queue so that it is available for consumption by the AQReader.
     * Once this function is called the claimed memory must no longer be
     * accessed or the results are undefined.
     *
     * @param item The item to commit.  When this function returns this item
     * is marked as not allocated (AQWriterItem::isAllocated() returns false).
     * @returns True if the commit succeeded or false if it fails; the commit
     * fails if the claimed item was treated as incomplete by the reader due
     * to begin held for a period longer than the commit timeout.
     * @throws invalid_argument If the passed item is invalid (i.e., is not 
     * currently claimed, is not in the queue, or is already committed).
     */
    bool commit(AQWriterItem& item);

private:

    // Performs a commit on a single item given by 'item'.
    bool commitSingle(AQItem& item);

public:

    // Performs a partial commit starting with the item at 'start' and ending 
    // at 'end'.  If 'start' is 0 then the commit initialisation
    // is performed if needed.
    //
    // This function exists to aid in unit testing.  Care must be taken that
    // all buffers are committed in order.
    bool commitExtendable(AQWriterItem& item, size_t start, size_t end);

private:

    // Configures all the link identifiers in the passed item.
    static void setExtendableLinkIdentifiers(AQWriterItem& item);

    // Defines all the available test points where event injection can occur.
public:

    // In claim() before the headRef field is written but after the
    // calculation has been made.
    static const int ClaimBeforeWriteHeadRef = 0;
        
    // In claim() before the control field for the skipped pages is
    // about the written.
    static const int ClaimBeforeWriteCtrlSkipPages = ClaimBeforeWriteHeadRef + 1;

    // In claim() before the control field for item is about to be
    // written.
    static const int ClaimBeforeWriteCtrl = ClaimBeforeWriteCtrlSkipPages + 1;

    // In commit() just before the control field is about to be written.
    static const int CommitBeforeWriteCtrl = ClaimBeforeWriteCtrl + 1;

    // Must the the last entry - the number of test points.
    static const int TestPointCount = CommitBeforeWriteCtrl + 1;

};




#endif
//=============================== End of File ==================================
