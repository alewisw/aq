//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQWriter.h"
#include "AQWriterItem.h"

#include "Crc32.h"
#include "CtrlOverlay.h"
#include "TraceBuffer.h"

#include <sstream>
#include <stdexcept>

using namespace std;
using namespace aq;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
AQWriter::AQWriter(void *mem, size_t memSize)
    : AQ(TestPointCount, mem, memSize)
{
}

//------------------------------------------------------------------------------
AQWriter::AQWriter(void *mem, size_t memSize, TraceBuffer *trace)
    : AQ(TestPointCount, mem, memSize, trace)
{
}

//------------------------------------------------------------------------------
AQWriter::AQWriter(const AQWriter& other)
    : AQ(other)
{
}

//------------------------------------------------------------------------------
AQWriter& AQWriter::operator=(const AQWriter& other)
{
    if (this != &other)
    {
        AQ::operator=(other);
    }
    return *this;
}

//------------------------------------------------------------------------------
AQWriter::~AQWriter(void)
{
}

//------------------------------------------------------------------------------
const volatile uint32_t& AQWriter::freeCounter(void) const
{
    CtrlOverlay *c = ctrlThrowOnUnformatted(__FUNCTION__);

    return c->freeCounter;
}

//------------------------------------------------------------------------------
size_t AQWriter::sizeToCapacity(size_t size) const
{
    size_t pageSize = 1 << m_ctrl->pageSizeShift;

    // Assumes pageSize is a power of 2 which must be true for AQWriter 
    // objects.
    size = (size + pageSize - 1) & ~(pageSize - 1);
    if (size == 0)
    {
        size = pageSize;
    }
    return size;
}

//------------------------------------------------------------------------------
bool AQWriter::claim(AQWriterItem& item, size_t memSize)
{
    // Obtain the control overlay; if it is not formatted then throw an 
    // exception indicating that we cannot process it.
    CtrlOverlay *c = ctrlThrowOnUnformatted(__FUNCTION__);

    // Extendable queues always allocate whole pages.
    if (c->options & AQ::OPTION_EXTENDABLE)
    {
        memSize = sizeToCapacity(memSize);
    }

    if (memSize < 1 || memSize > CtrlOverlay::CTRLQ_SIZE_MASK)
    {
        ostringstream ss;
        ss << "Cannot claim memory of size " << memSize 
           << " as it is not in the range [1, " << CtrlOverlay::CTRLQ_SIZE_MASK << "]";
        TRACE_INVALID("%s", ss.str().c_str());
        throw invalid_argument(ss.str());
    }

    uint32_t requiredPages = c->sizeToPageCount(memSize);
    TRACE_CTRL_ENTRY(c, "%u bytes -> %u pgs", (unsigned int)memSize, (unsigned int)requiredPages);

    uint32_t currHeadRef;   // The head value at the start of the loop.
    uint32_t currHead;      // The head value index.
    uint32_t nextHeadRef;   // The next head reference after claim.
    uint32_t skipPages = 0; // The number of pages to skip to make a
                            // sequential allocation.
    currHeadRef = Atomic::read(&c->headRef);
    for (;;)
    {
        uint32_t cmpHeadRef;

        // Must read head first, then read tail.  Tail can only change to give
        // us more space but head could change to give us less.
        uint32_t currTailRef = Atomic::read(&c->tailRef);

        // We define two paths - the fast path and the slow path.  The fast path has less 
        // operations than the slow path, however it is only valid when we can prove that
        // the next write will fit into the contiguous memory starting at 'head'.
        //
        // There are two cases:
        //   headIdx >= tailIdx --> continguous space is (headIdx - pageCount - 1)
        //   headIdx <  tailIdx --> noting that the SEQ on head will be one more than on tail,
        //                          contiguous space is (tail - head) & INDEX_MASK
        // Luckily, for both calculations, we get a 'very big number' when the precondition
        // is not hte case (headIdx >= tailIdx, the calculation (tail - head) & INDEX_MASK gives
        // a large number.  Thus we just blindly test less than in both cases.
        currHead = currHeadRef & CtrlOverlay::REF_INDEX_MASK;
        if (   requiredPages < ((currTailRef - currHeadRef) & CtrlOverlay::REF_INDEX_MASK)
            && requiredPages < c->pageCount - currHead)
        {
            // --- FAST PATH ---
            nextHeadRef = currHeadRef + requiredPages;
            testPoint(ClaimBeforeWriteHeadRef);
            cmpHeadRef = Atomic::cmpXchg(&c->headRef, nextHeadRef, currHeadRef);
            if (cmpHeadRef == currHeadRef)
            {
                skipPages = 0;
                break;
            }
        }
        else
        {
            // --- SLOW PATH ---
            uint32_t currTail = c->queueRefToIndex(currTailRef);
            if (currHead < currTail)
            {
                // We must allocate contiguous memory; if head is less than tail
                // then the available pages are all contiguous.
                uint32_t availPages = currTail - currHead - 1;
                skipPages = 0;
                if (availPages < requiredPages)
                {
                    // Out of space - cannot allocate.  It is believed, but not proved,
                    // that this must ALWAYS be true if the fast path conditions were 
                    // not met.  TODO: further investigation.
                    TRACE_CTRL_EXIT(c, "out of space H[%u]->T[%u]: %u of %u", 
                                    currHead, currTail, availPages, requiredPages);
                    item.clear();
                    return false;
                }
            }
            else
            {
                // Calculate the available contiguous pages at the end of the queue.
                uint32_t endPages = pageCount() - currHead - (currTail == 0 ? 1 : 0);
                if (endPages < requiredPages)
                {
                    // There is not enough space for contiguous allocation; test if
                    // there is enough at the front of the queue.  If there is
                    // enough then we must skip the end otherwise just exit with
                    // out of memory.
                    if (currTail < requiredPages + 1)
                    {
                        TRACE_CTRL_EXIT(c, "out of space H[%u]->T[%u]: (%u or %u - 1) of %u",
                                        currHead, currTail, endPages, currTail, requiredPages);
                        item.clear();
                        return false;
                    }
                    skipPages = endPages;
                }
                else
                {
                    skipPages = 0;
                }
            }
            nextHeadRef = c->queueRefIncrement(currHeadRef, skipPages + requiredPages);
            testPoint(ClaimBeforeWriteHeadRef);
            cmpHeadRef = Atomic::cmpXchg(&c->headRef, nextHeadRef, currHeadRef);
            if (cmpHeadRef == currHeadRef)
            {
                break;
            }
        }
        currHeadRef = cmpHeadRef;
        Atomic::increment(&c->claimContention);
    } 


    // Memory successfully allocated; the skip pages need to have the 'discard' and
    // 'commit' bits set (but not 'claim' as they were never claimed) to indicate 
    // that the entire range should be discarded and not returned as an item.
    if (skipPages > 0)
    {
        // Zero the control queue after the first to indicate the are not used.
        // This is critical for valid snapshot recovery.
        testPoint(ClaimBeforeWriteCtrlSkipPages);
        for (uint32_t i = 1; i < skipPages; ++i)
        {
            Atomic::write(&c->ctrlq[currHead + i], 0);
        }
        Atomic::write(&c->ctrlq[currHead],   CtrlOverlay::CTRLQ_DISCARD_MASK 
                                           | CtrlOverlay::CTRLQ_COMMIT_MASK
                                           | (currHeadRef & CtrlOverlay::CTRLQ_SEQ_MASK)
                                           | (skipPages << c->pageSizeShift));
        currHeadRef = (currHeadRef & CtrlOverlay::REF_SEQ_MASK) + CtrlOverlay::REF_SEQ_INCR;
        currHead = 0;
    }

    // Zero the control queue after the first to indicate the are not used.
    // This is critical for valid snapshot recovery.
    testPoint(ClaimBeforeWriteCtrl);
    for (uint32_t i = 1; i < requiredPages; ++i)
    {
        Atomic::write(&c->ctrlq[currHead + i], 0);
    }

    // Finally mark the size into the head control queue entry and return the
    // memory.
    uint32_t ctrlVal = memSize | (currHeadRef & CtrlOverlay::CTRLQ_SEQ_MASK) 
                               | CtrlOverlay::CTRLQ_CLAIM_MASK;
    Atomic::write(&c->ctrlq[currHead], ctrlVal);

    item.m_ctrl = ctrlVal;
    item.m_mem = c->pageToMem(currHead);
    item.m_memSize = (c->options & AQ::OPTION_EXTENDABLE) ? 0 : memSize;
    item.m_quid = currHeadRef & AQItem::QUEUE_IDENTIFIER_MASK;
    item.m_lkid = AQItem::QUEUE_IDENTIFIER_INVALID;
    item.m_writer = this;
    item.m_accumulator = 0;

    if (skipPages == 0)
    {
        TRACE_1ITEM_EXIT(c, &item);
    }
    else
    {
        TRACE_1ITEM_EXIT(c, &item, "skip<%u>", skipPages);
    }
    return true;
}

//------------------------------------------------------------------------------
bool AQWriter::commit(AQWriterItem& item)
{
    if ((m_ctrl->options & AQ::OPTION_EXTENDABLE) == 0)
    {
        bool res = commitSingle(item);
        item.clear();
        return res;
    }
    else
    {
        return commitExtendable(item, 0, 0xFFFFFFFE);
    }
}

//------------------------------------------------------------------------------
bool AQWriter::commitSingle(AQItem& item)
{
    CtrlOverlay *c = m_ctrl;
    uint32_t pageNum = c->memToPage(item.m_mem);
    if (pageNum == CtrlOverlay::PAGENUM_INVALID)
    {
        ostringstream ss;
        ss << "Item passed to " << __FUNCTION__ << " invalid, memory address "
            << (void *)item.m_mem << " is not within the page range";
        TRACE_INVALID("%s", ss.str().c_str());
        item.m_first->clear();
        throw invalid_argument(ss.str());
    }
    uint32_t extPageNum = pageNum;

    // Store the link ID if link IDs are enabled.
    if (m_ctrl->options & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER)
    {
        extPageNum += c->pageCount;

        c->ctrlq[extPageNum] = item.linkIdentifier();
    }

    // Calculate and store the CRC if CRC's are enabled.
    uint32_t crc = 0;
    if (m_ctrl->options & OPTION_CRC32)
    {
        extPageNum += c->pageCount;

        c->ctrlq[extPageNum] = crc = CalculateItemCrc32(item, m_ctrl->options);
    }

    // Mark the entry as committed; this means that the consumer can now see and
    // consume this entry from the queue.
    //
    // We must make sure that the consumer has not:
    //  a) Returned an incomplete item by setting the DISCARD bit, or
    //  b) Discarded the item and set the value to 0.
    //
    // If this has happend then it could have been re-allocated, however we use
    // the SEQ field to protect against multiple re-allocations of the same 
    // size.
    //
    // In this case we probably have memory corruption in the queue because the
    // committer was probably writing to memory that had been reallocated to 
    // some other requestor.  However there is nothing we can do about that 
    // possibility other than rely on the claim() -> commit() time being 
    // less than the garbage collection time configured for the queue.
    //
    // At the very least we can catch the error and report it with this 
    // exhange.
    uint32_t baseCtrl = item.m_ctrl;
    item.m_ctrl |= CtrlOverlay::CTRLQ_COMMIT_MASK;
    testPoint(CommitBeforeWriteCtrl);
    uint32_t cmpCtrl = Atomic::cmpXchg(&c->ctrlq[pageNum], item.m_ctrl, baseCtrl);
    bool res = cmpCtrl == baseCtrl;
    if (res)
    {
        Atomic::increment(&c->commitCounter);
        if (m_ctrl->options & OPTION_CRC32)
        {
            TRACE_1ITEMDATA_ENTRYEXIT(c, &item, "crc[%08X]", crc);
        }
        else
        {
            TRACE_1ITEMDATA_ENTRYEXIT(c, &item);
        }
    }
    else
    {
        TRACE_1ITEMDATA_ENTRYEXIT(c, &item, "failed, ctrl was 0x%08X vs req 0x%08X", cmpCtrl, baseCtrl);
    }
    return res;
}

//------------------------------------------------------------------------------
bool AQWriter::commitExtendable(AQWriterItem& item, size_t start, size_t end)
{
    // In extendable mode configure the link identifiers.
    if (start == 0 || item.linkIdentifier() == AQItem::QUEUE_IDENTIFIER_INVALID)
    {
        setExtendableLinkIdentifiers(item);
    }

    if (start > 0 || end < 0xFFFFFFFE)
    {
        TRACE_ITEM_ENTRY(m_ctrl, &item, "indexes[%u->%u]", (unsigned int)start, (unsigned int)end);
    }
    else
    {
        TRACE_ITEM_ENTRY(m_ctrl, &item);
    }

    AQItem *curr = &item;
    size_t pos = 0;
    while (curr != NULL)
    {
        if (pos > end)
        {
            TRACE_CTRL_EXIT(m_ctrl, "early exit");
            return true;
        }
        else if (pos >= start)
        {
            if (!commitSingle(*curr))
            {
                item.clear();
                TRACE_CTRL_EXIT(m_ctrl, "commit failed, abandon remainder of item");
                return false;
            }
        }
        curr = curr->m_next;
        pos++;
    }
    item.clear();
    TRACE_CTRL_EXIT(m_ctrl, "commit succeeded");
    return true;
}

//------------------------------------------------------------------------------
void AQWriter::setExtendableLinkIdentifiers(AQWriterItem& item)
{
    AQItem *curr = &item;
    AQItem *next = NULL;
    while ((next = curr->m_next) != NULL)
    {
        curr->m_lkid = next->m_quid;
        curr = next;
    }
    curr->m_lkid = curr->size() | AQItem::LINK_IDENTIFIER_LAST;
    item.m_lkid |= AQItem::LINK_IDENTIFIER_FIRST;
}




//=============================== End of File ==================================
