#ifndef CTRLOVERLAY_H
#define CTRLOVERLAY_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Atomic.h"




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

// Memory overlay structure for the Multi-Producer Allocating Concurrent queue.
//
// The queue memory is structured as follows:
//
//        <------------------------------->
//                pageCount pages
// 
//        +---+---+---+---+---+---+---+---+ 
// ctrlq  | 0 |*1*|*2*|*3*|*4*| 5 | 6 | 7 | <--- 1 word per page
//        +---+---+---+---+---+---+---+---+
//              ^               ^    
//              |               |                              
//              tail            head
//              |               |
//              V               V
//        +---+---+---+---+---+---+---+---+
// memq   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | <--- (1 << pageSizeShift) bytes per page
//        |   |   |   |   |   |   |   |   |
//        |   |   |   |   |   |   |   |   |
//        |   |   |   |   |   |   |   |   |
//        |   |   |   |   |   |   |   |   |
//        +---+---+---+---+---+---+---+---+
//
// pageSizeShift is the log2 of the size of each page.
//
// The head and tail are stored as 32-bit words:
//
// +--------------+----------------------------+
// |   Sequence   |           Index            |
// +--------------+----------------------------+
//      12-bit                20-bit
//
// The fields are:
//  Index:    The index into the circular buffer of the head or tail.
//  Sequence: A field that increments by 1 each time the index wraps from
//            pageCount-1 to 0.  This is used to protect the allocation of 
//            new items from the wrap-around problem.
//
//       
// Each ctrlq entry is a single 32-bit word.  The word is structured as follows:
//
// +-------+-------+-------+----------+----------------------------+
// | Claim |Commit |Discard| Sequence |       Allocated Size       |
// +-------+-------+-------+----------+----------------------------+
//   1-bit   1-bit   1-bit    9-bit              20-bit
//
// The fields are:
//  Claim:    A single bit, if set to '1' then this entry is the start of a
//            (possily multi-page) item.  If set to '0' then this item
//            is either not allocated (outside head->tail) or if inside
//            head->tail it is:
//             a) Part of a multi-page item.
//             b) The start of an item where the ctrlq word is yet to be
//                written.
//  Commit:   A single bit, if set to '1' then this is the start of a
//            multi-part item that has been completed and committed to 
//            the queue.
//  Discard:  A single bit, if set to '1' then this item has been processed
//            and released.  The tail may move over it to release the 
//            used items.
//  Sequence: The bottom 9-bits of the headRef sequence number when this 
//            item was written into the queue.
//  Size:     The total number of bytes requested for allocation to this
//            itemin the call to claim().
// The Claim/Commit/Discard bits essentially form the state of the page; they
// thus can be:
//   --- = The item has not been allocated since queue formatting or is part
//         of an internal page within a multi-page item.
//   c-- = The item has been claimed from a producer, but not yet committed 
//         back into the queue.
//   -C- = The item has been claimed and committed by a producer, then retrieved,
//         released and free'd by the consumer.
//   cC- = The item has been claimed from a producer and has been committed
//         back into the queue read for retreival by the consumer.
//   --D = [special] The item is 'waste' at the end of the array and has now 
//         been free'd by the consumer.
//   c-D = Alternatly the item was retreived by the consumer as an incomplete
//         item and has subsequently been released but not yet free'd back 
//         into the queue.
//   -CD = [special] The item is 'waste' at the end of the array that could not
//         be used as the claim was larger than the available space.  The item has
//         not yet been free'd by the consumer.
//   cCD = The item has been claimed and committed by a producer, then retrieved
//         and released by the consumer but not yet free'd.
namespace aq { struct CtrlOverlay
{

public:

    //--------------------------------------------------------------------------
    // START OF MEMORY OVERLAY REGION
    //--------------------------------------------------------------------------

    // The format version number, set when the memory is headerXref.
    uint32_t formatVersion;

    // The options bit-mask used to indicate which optional features have
    // been enabled.
    uint32_t options;

    // Defines the total size of the memory region.
    uint32_t size;

    // Defines the log2 of the page size; the total size of each page is
    // (1 << 'pageSizeShift') bytes.
    uint32_t pageSizeShift;

    // The total number of pages in the memory region.
    uint32_t pageCount;

    // The offset of the memory region from the start of this structure.
    uint32_t memOffset;

    // The timeout for a commit operation in milliseconds.
    uint32_t commitTimeoutMs;

    // Used as an indicator of formatting status; merges all of the fixed
    // fields in this header as a check-sum along with a magic number.
    uint32_t headerXref;

    // Montonic counter increments by 1 each time contention is detected
    // at claim time.
    uint32_t claimContention;

    // Monotonic counter increments by 1 each time a commit occurs.
    uint32_t commitCounter;

    // Monotonic counter increments by 1 each time a page is free'd.
    uint32_t freeCounter;

    // The following volatile fields must be placed here - they are handled
    // specially when taking a snapshot.  New header fields must be placed
    // above this position.

    // The reference counter for the current head position in the queue.
    volatile uint32_t headRef;

    // The reference counter for the current tail position in the queue.
    volatile uint32_t tailRef;

    // The control queue with one entry for each page (ie., there are 
    // pageCount entries).
    volatile uint32_t ctrlq[1];

    //--------------------------------------------------------------------------
    // END OF MEMORY OVERLAY REGION
    //--------------------------------------------------------------------------




    // Used in the 'formatVersion' field to indicate that this memory is not
    // formatted.
    static const uint32_t FORMAT_VERSION_INVALID = 0x00000000;

    // Used in the 'formatVersion' field to indicate the V1 format.
    static const uint32_t FORMAT_VERSION_1 = 0x00000001;
    
    // The shift for the page size field in the headerXref mask.
    static const int HEADER_XREF_PAGE_SIZE_SHIFT = 26;

    // The shift for the memory offset field in the headerXref mask.
    static const int HEADER_XREF_MEM_OFFSET_SHIFT = 14;

    // The maximum permitted number of pages.
    static const uint32_t PAGE_COUNT_MAX = 0x00100000;

    // The alignment for each page, in log2 bytes.  That is the actual 
    // alignment is given by 1 << PAGE_ALIGN_SHIFT.
    static const size_t PAGE_ALIGN_SHIFT = 6;

    // The mask for the control queue claim bit.
    static const uint32_t CTRLQ_CLAIM_MASK = 0x80000000;

    // The mask for the control queue commit bit. 
    static const uint32_t CTRLQ_COMMIT_MASK = 0x40000000;

    // The mask for the control queue discard bit.
    static const uint32_t CTRLQ_DISCARD_MASK = 0x20000000;

    // The mask for the control queue flags.
    static const uint32_t CTRLQ_FLAGS_MASK = CTRLQ_CLAIM_MASK | CTRLQ_COMMIT_MASK | CTRLQ_DISCARD_MASK;

    // The mask for the control queue size field.
    static const uint32_t CTRLQ_SIZE_MASK = 0x000FFFFF;

    // The mask for the control queue sequence field.
    static const uint32_t CTRLQ_SEQ_MASK = 0x1FF00000;

    // Defines an invalid page number.
    static const uint32_t PAGENUM_INVALID = 0xFFFFFFFF;

    // The mask used to extract the index from a cell reference.
    static const uint32_t REF_INDEX_MASK = 0x000FFFFF;

    // The mask used to extract the sequence from a cell reference.
    static const uint32_t REF_SEQ_MASK = 0xFFF00000;

    // The amount to increment the cell reference on each rotation of the index.
    static const uint32_t REF_SEQ_INCR = 0x00100000;

    // The shift to reach the SEQ field in the reference.
    static const uint32_t REF_SEQ_SHIFT = 20;

    // Returns true if this control overlay has been formatted for the specified
    // memory size.
    bool isFormatted(size_t memSize) const;

    // Returns a pointer to the memory for the page given by 'pageNum'.
    unsigned char *pageToMem(uint32_t pageNum) const;

    // Returns the page number for a particular memory address 'mem'.  If the 
    // memory address is not for a valid page then PAGENUM_INVALID is returned.
    uint32_t memToPage(const void *mem) const;

    // Returns the number of pages required for a particular memory size 
    // 'memSize'.
    uint32_t sizeToPageCount(size_t memSize) const;

    // Returns the correct index into a queue for a particular queue reference 
    // 'ref'.
    uint32_t queueRefToIndex(uint32_t ref) const;

    // Increments a queue reference 'ref' by the amount 'amount' returning the
    // newly increment queue reference.
    uint32_t queueRefIncrement(uint32_t ref, uint32_t amount) const;

    // Returns the number of sequential pages availableSize in the queue given
    // the passed head index and tail index values.
    uint32_t availableSequentialPages(uint32_t headIdx, uint32_t tailIdx) const;

}; }





#endif
//=============================== End of File ==================================
