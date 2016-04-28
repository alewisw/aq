//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQReader.h"
#include "AQItem.h"

#include "Crc32.h"
#include "CtrlOverlay.h"
#include "LinkedItemProcessor.h"
#include "Timer.h"
#include "TraceBuffer.h"

#include <cstddef>
#include <cstring>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace aq;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// Set this to enable quick advancing via the pstate skipCount field.
#define ADVANCE_WITH_PSTATE

#ifdef AQ_TEST_TRACE

#define TRACE_PSTATE_ITEM(ref)                                                                  \
    AQItem _pitem;                                                                      \
    _pitem.m_ctrl    = m_ctrl->ctrlq[(ref) & CtrlOverlay::REF_INDEX_MASK];                      \
    _pitem.m_quid    = (ref);                                                                   \
    _pitem.m_memSize = _pitem.m_ctrl & CtrlOverlay::CTRLQ_SIZE_MASK;                            \
    _pitem.m_mem     = m_ctrl->pageToMem((ref) & CtrlOverlay::REF_INDEX_MASK)

// Used to trace the processor state for index 'idx' using the display 'code'.
#define TRACE_PSTATE(ref, code)                                                                 \
do                                                                                              \
{                                                                                               \
    TRACE_PSTATE_ITEM(ref);                                                                     \
    TRACE_1ITEM(m_ctrl, &_pitem, " pstate[Q%u:%u] %s %lu ms / %c%c%c / %u",                      \
          ((ref) & CtrlOverlay::REF_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT,                    \
          ((ref) & CtrlOverlay::REF_INDEX_MASK),                                                \
          code,                                                                                 \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerStarted                            \
            ? Timer::elapsed(m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerStartMs) : 0,   \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerStarted ? 'S' : '-',               \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerExpired ? 'X' : '-',               \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].retrieved ? 'R' : '-',                  \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].skipCount);                             \
} while (0)

#define TRACE_PSTATE2(ref, code, skipIdx)                                                       \
do                                                                                              \
{                                                                                               \
    TRACE_PSTATE_ITEM(ref);                                                                     \
    TRACE_1ITEM(m_ctrl, &_pitem, " pstate[Q%u:%u] %s %lu ms / %c%c%c / %u / skip-update %u",     \
          ((ref) & CtrlOverlay::REF_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT,                    \
          ((ref) & CtrlOverlay::REF_INDEX_MASK),                                                \
          code,                                                                                 \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerStarted                            \
            ? Timer::elapsed(m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerStartMs) : 0,   \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerStarted ? 'S' : '-',               \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].timerExpired ? 'X' : '-',               \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].retrieved ? 'R' : '-',                  \
          m_pstate[(ref) & CtrlOverlay::REF_INDEX_MASK].skipCount,                              \
          skipIdx);                                                                             \
} while (0)

#else

#define TRACE_PSTATE(ref, code)
#define TRACE_PSTATE2(ref, code, skipIdx)

#endif




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
AQReader::AQReader(void *mem, size_t memSize)
    : AQ(TestPointCount, mem, memSize)
    , m_pstate(NULL)
    , m_linkProcessor(NULL)
{
}

//------------------------------------------------------------------------------
AQReader::AQReader(void *mem, size_t memSize, aq::TraceBuffer *trace)
    : AQ(TestPointCount, mem, memSize, trace)
    , m_pstate(NULL)
    , m_linkProcessor(NULL)
{
}

//------------------------------------------------------------------------------
AQReader::~AQReader(void)
{
    if (m_pstate)
    {
        delete[] m_pstate;
        m_pstate = NULL;
    }
    if (m_linkProcessor)
    {
        delete m_linkProcessor;
    }
}

//------------------------------------------------------------------------------
bool AQReader::format(uint32_t pageSizeShift, uint32_t commitTimeoutMs,
                      uint32_t options)
{
    // The memory layout is as follows:
    //
    // +-----------+  <-- Start of memory.
    // |  Control  |
    // +-----------+
    // | ctrlq[ 0] |
    // | ctrlq[ 0] |
    // |    ...    |
    // | ctrlq[ n] |
    // +-----------+  --\
    // | crc32[ 0] |     |
    // | crc32[ 0] |     | Optional if CRC32 is enabled, one per page
    // |    ...    |     |  OPTIONS_FLAG_CRC32
    // | crc32[ n] |     |
    // +-----------+  --/
    // |  Padding  |
    // +-----------+  <-- Aligned to a min(1 << pageSizeBytes, 64)-byte boundary.
    // |  memq[0]  |
    // |           |
    // +-----------+
    // |  memq[1]  |
    // |           |
    // +-----------+
    //      ...
    // +-----------+
    // |  memq[n]  |
    // |           |
    // +-----------+
    // |   Waste   |
    // +-----------+
    CtrlOverlay *c = m_ctrl;
    if (c == NULL || m_memSize < offsetof(CtrlOverlay, headerXref) + sizeof(c->headerXref))
    {
        return false;
    }
    options = options & CtrlOverlay::OPTION_VALID_MASK;
    
    // Make the control overlay completly invalid before modifying anything else.
    Atomic::write(&c->formatVersion, CtrlOverlay::FORMAT_VERSION_INVALID);
    Atomic::write(&c->options, 0xFFFFFFFF);
    Atomic::write(&c->headerXref, 0);

    uint32_t ctrlqMultiplier = 1;
    
    if (options & OPTION_CRC32)
    {
        ctrlqMultiplier++;
    }
    if (options & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER)
    {
        ctrlqMultiplier++;
    }

    size_t overhead = sizeof(*c) - sizeof(c->ctrlq);
    size_t pageSize = (1 << pageSizeShift) + sizeof(c->ctrlq[0]) * ctrlqMultiplier;
    size_t alignShift = pageSizeShift < CtrlOverlay::PAGE_ALIGN_SHIFT
        ? pageSizeShift : CtrlOverlay::PAGE_ALIGN_SHIFT;
    size_t alignMask = (1 << alignShift) - 1;


    // Calculate the maximum possible page size, then reduce the page size until
    size_t pageCount = (m_memSize - overhead) / pageSize;
    if (pageCount > CtrlOverlay::PAGE_COUNT_MAX)
    {
        pageCount = CtrlOverlay::PAGE_COUNT_MAX;
    }

    // Calculate the required memory queue address for this page size aligned 
    // to the appropriate boundary address.
    size_t memqEnd = (size_t)c + m_memSize;
    size_t memqStart = 0;
    for (;;)
    {
        memqStart = ((size_t)c + overhead + pageCount * sizeof(c->ctrlq[0]) * ctrlqMultiplier + alignMask) & ~alignMask;
        if (memqStart + (pageCount << pageSizeShift) > memqEnd)
        {
            pageCount--;
        }
        else
        {
            break;
        }
    }
    if (pageCount <= 1)
    {
        return false;
    }

    // Construct the memory region.
    c->size = m_memSize;
    c->pageSizeShift = pageSizeShift;
    c->pageCount = pageCount;
    c->memOffset = memqStart - (size_t)c;
    c->commitTimeoutMs = commitTimeoutMs;
    c->claimContention = 0;
    c->commitCounter = 0;
    c->freeCounter = 0;
    c->headRef = 0;
    c->tailRef = 0;
    memset((void *)c->ctrlq, 0, ctrlqMultiplier * pageCount * sizeof(c->ctrlq[0]));

    // Mark it formatted.
    Atomic::write(&c->headerXref, ((c->pageSizeShift << CtrlOverlay::HEADER_XREF_PAGE_SIZE_SHIFT)
            | (c->memOffset << CtrlOverlay::HEADER_XREF_MEM_OFFSET_SHIFT)
            | (c->pageCount)));

    // Make the control overlay valid.
    Atomic::write(&c->options, options);
    Atomic::write(&c->formatVersion, CtrlOverlay::FORMAT_VERSION_1);

    // Create the page state queue.
    if (m_pstate)
    {
        delete[] m_pstate;
    }
    m_pstate = new PageState[c->pageCount];
    memset(m_pstate, 0, sizeof(PageState) * c->pageCount);

    // Create the link processor if one is needed.
    if (options & OPTION_EXTENDABLE)
    {
        if (m_linkProcessor)
        {
            m_linkProcessor->reset();
        }
        else
        {
            m_linkProcessor = new LinkedItemProcessor;
        }
    }
    else if (m_linkProcessor)
    {
        delete m_linkProcessor;
        m_linkProcessor = NULL;
    }
    return true;
}

//------------------------------------------------------------------------------
const volatile uint32_t& AQReader::commitCounter(void) const
{
    CtrlOverlay *c = ctrlThrowOnUnformatted(__FUNCTION__);

    return c->commitCounter;
}

//------------------------------------------------------------------------------
bool AQReader::retrieve(AQItem& item)
{
    bool res = true;
    CtrlOverlay *c = ctrlThrowOnUnformatted(__FUNCTION__);

    TRACE_CTRL_ENTRY(c);
    if (m_linkProcessor == NULL)
    {
        if (!(res = walk(&item)))
        {
            item.clear();
        }
    }
    else
    {
        res = false;
        while (!res && walk(&item))
        {
            switch (m_linkProcessor->nextItem(item))
            {
            case LinkedItemProcessor::DISCARD:
                release(item);
                break;

            case LinkedItemProcessor::PRODUCED:
                res = true;
                break;
            }
        }
        if (!res)
        {
            item.clear();
        }
    }
    TRACE_ITEMDATA_EXIT(c, &item);
    return res;
}

//------------------------------------------------------------------------------
void AQReader::release(AQItem& item)
{
    if (m_linkProcessor)
    {
        releaseExtendable(item, 0, 0xFFFFFFFE);
    }
    else
    {
        TRACE_ITEM_ENTRY(m_ctrl, &item);
        releaseSingle(item);

        // Perform discard operations to clean-up the ring-buffer.
        walk();
        TRACE_EXIT();
        item.clear();
    }
}

//------------------------------------------------------------------------------
void AQReader::releaseSingle(AQItem& item)
{
    // Note: we don't check for the CtrlOverlay being formatted as it MUST be
    //       formatted if there was valid retrieve().
    CtrlOverlay *c = m_ctrl;

    uint32_t pageNum = c->memToPage(item.m_mem);
    if (pageNum == CtrlOverlay::PAGENUM_INVALID)
    {
        ostringstream ss;
        ss << "Item passed to " << __FUNCTION__ << " invalid, memory address "
            << (void *)item.m_mem << " is not within the page range";
        TRACE_INVALID("%s", ss.str().c_str());
        item.clear();
        throw invalid_argument(ss.str());
    }

    // Mark the page for discard.
    uint32_t cmpCtrl = Atomic::cmpXchg(&c->ctrlq[pageNum],
        item.m_ctrl | CtrlOverlay::CTRLQ_DISCARD_MASK, item.m_ctrl);
    if (cmpCtrl != item.m_ctrl)
    {
        if ((item.m_ctrl | CtrlOverlay::CTRLQ_COMMIT_MASK) == cmpCtrl)
        {
            // The item was committed while it was processed as
            // incomplete; we must mark it as DISCARD.
            item.m_ctrl |= CtrlOverlay::CTRLQ_COMMIT_MASK;
            testPoint(ReleaseBeforeWriteSecondCtrl);
            cmpCtrl = Atomic::cmpXchg(&c->ctrlq[pageNum],
                item.m_ctrl | CtrlOverlay::CTRLQ_DISCARD_MASK, item.m_ctrl);
            if (cmpCtrl != item.m_ctrl)
            {
                TRACE_1ITEM_INVALID(c, &item, "release failed after set commit bit, ctrl was 0x%08X vs req 0x%08X",
                                    cmpCtrl, item.m_ctrl);
                ostringstream ss;
                ss << "Item passed to " << __FUNCTION__ << " invalid";
                item.m_first->clear();
                throw invalid_argument(ss.str());
            }
        }
        else
        {
            TRACE_1ITEM_INVALID(c, &item, "release failed, ctrl was 0x%08X vs req 0x%08X",
                                cmpCtrl, item.m_ctrl);
            ostringstream ss;
            ss << "Item passed to " << __FUNCTION__ << " invalid";
            item.m_first->clear();
            throw invalid_argument(ss.str());
        }
    }
    TRACE_1ITEM_ENTRYEXIT(c, &item);
}

//------------------------------------------------------------------------------
void AQReader::releaseExtendable(AQItem& item, size_t start, size_t end)
{
    if (start > 0 || end < 0xFFFFFFFE)
    {
        TRACE_ITEM_ENTRY(m_ctrl, &item, "indexes[%u->%u]", (unsigned int)start, (unsigned int)end);
    }
    else
    {
        TRACE_ITEM_ENTRY(m_ctrl, &item);
    }

    size_t idx = 0;
    for (AQItem *curr = &item; curr != NULL; curr = curr->m_next)
    {
        if (idx > end)
        {
            TRACE_CTRL_EXIT(m_ctrl, "early exit");
            return;
        }
        else if (idx >= start)
        {
            releaseSingle(*curr);
        }
        idx++;
    }
    walk();
    item.clear();
    TRACE_CTRL_EXIT(m_ctrl);
}

//------------------------------------------------------------------------------
bool AQReader::walk(AQItem *item)
{
#ifdef AQ_TEST_POINT
    int walkAfterReadCtrlN = WalkAfterReadCtrlN;
#endif
    CtrlOverlay *c = m_ctrl;
    
    // Read the head and tail; we won't read them again after this point.
    uint32_t initTailRef = Atomic::read(&c->tailRef);
    uint32_t currHeadRef = Atomic::read(&c->headRef);
    uint32_t currTailRef = initTailRef;
    uint32_t nextTailRef = initTailRef;

    // The index into the pstate array where we perform skip updates.
    uint32_t pstateSkipUpdateRef = initTailRef;
    uint32_t pstateSkipUpdateIdx = c->queueRefToIndex(initTailRef);
    while (currHeadRef != currTailRef)
    {
        /*
        if ((currTailRef > currHeadRef) && !((currTailRef & CtrlOverlay::REF_SEQ_MASK) == 0xFFF00000 && (currHeadRef & CtrlOverlay::REF_SEQ_MASK) == 0x00000000))
        {
            TRACE_CTRL(c, "$$$$$$ currTailRef[%08X] > currHeadRef[%08X]", currTailRef, currHeadRef);
            WALK_ERROR_FOUND = true;
        }
        */

        // Determine the state of this item.
        uint32_t currTail = c->queueRefToIndex(currTailRef);
        uint32_t ctrlTail = Atomic::read(&c->ctrlq[currTail]);
        TRACE_PSTATE2(currTailRef, "==", pstateSkipUpdateIdx);
#ifdef AQ_TEST_POINT
        testPoint(walkAfterReadCtrlN++);
#endif

        // Calculate the size of the item and the number of pages to advance
        // for this item.
        uint32_t ctrlSize = 0;
        uint32_t ctrlPageCount = 1;
        uint32_t ctrlFlags = ctrlTail & CtrlOverlay::CTRLQ_FLAGS_MASK;
        if (   ctrlFlags == 0 
            || ctrlFlags == CtrlOverlay::CTRLQ_COMMIT_MASK 
            || ctrlFlags == CtrlOverlay::CTRLQ_DISCARD_MASK 
            || (ctrlTail & CtrlOverlay::CTRLQ_SEQ_MASK) != (currTailRef & CtrlOverlay::CTRLQ_SEQ_MASK))
        {
            // Any of these conditions indicate that the control queue item has
            // not yet been written.  Clear the flags and ctrlTail field internally.
            ctrlFlags = 0;
            ctrlTail = 0;
        }
        else
        {
            // The control field is valid - update the size and control page count.
            // If the skip size is not configured then set it correctly now as we
            // know the value for this page.
            ctrlSize = ctrlTail & CtrlOverlay::CTRLQ_SIZE_MASK;
            ctrlPageCount = c->sizeToPageCount(ctrlSize);
            if (!m_pstate[currTail].skipCount)
            {
                m_pstate[currTail].skipCount = ctrlPageCount;
                TRACE_PSTATE(currTailRef, "->");
            }
        }

        if (!m_pstate[currTail].retrieved && ctrlFlags == (  CtrlOverlay::CTRLQ_COMMIT_MASK 
                                                           | CtrlOverlay::CTRLQ_CLAIM_MASK))
        {
            // This item has been committed; it can be returned as a complete
            // item.  Update its information in the skip-queue.
            if (currTail != pstateSkipUpdateIdx)
            {
                m_pstate[pstateSkipUpdateIdx].skipCount += m_pstate[currTail].skipCount;
                TRACE_PSTATE(pstateSkipUpdateRef, "->");
            }
            return walkEnd(item, currTailRef, ctrlPageCount, ctrlSize);
        }

        // We know how many pages to advance the current tail; now we must determine if
        // the next tail should also be advanced as we are discarding items.
        bool discard = false;
        if (   (ctrlFlags & CtrlOverlay::CTRLQ_DISCARD_MASK) 
            && (ctrlFlags != CtrlOverlay::CTRLQ_DISCARD_MASK))
        {
            if (!(ctrlFlags & CtrlOverlay::CTRLQ_CLAIM_MASK))
            {
                // Waste - first time encountered - update skip counter and move to the next
                // page.
                if (currTail != pstateSkipUpdateIdx)
                {
                    m_pstate[pstateSkipUpdateIdx].skipCount += m_pstate[currTail].skipCount;
                    TRACE_PSTATE(pstateSkipUpdateRef, "->");
                }
            }
            if (nextTailRef == currTailRef)
            {
                // The item is 'marked for discard' it can be discarded as
                // it must have already been release()'ed or was marked for 
                // discard in a producer at the end of the ring-buffer.
                TRACE("free-discard pg<%u-%u>", currTail, currTail + ctrlPageCount - 1);
                discard = true;
            }
        }
        else
        {
            // Not marked for discard, but we cannot return it.  Start the
            // discard timer.
            if (!m_pstate[currTail].timerStarted)
            {
                m_pstate[currTail].timerStarted = 1;
                m_pstate[currTail].timerStartMs = Timer::start();
                TRACE_PSTATE(currTailRef, "->");
            }
            else if (!m_pstate[currTail].timerExpired)
            {
                Timer::Ms_t elapsedMs = Timer::elapsed(m_pstate[currTail].timerStartMs);
                if (elapsedMs > c->commitTimeoutMs)
                {
                    m_pstate[currTail].timerExpired = 1;
                    TRACE_PSTATE(currTailRef, "->");
                }  
            }

            // If there is less than 25% space available, and the incomplete
            // timer has expired then discard the item.
            uint32_t availPages = c->availableSequentialPages(
                c->queueRefToIndex(currHeadRef), currTail);
            uint32_t limitPages = ((pageCount() + 3) >> 2);
            if (availPages < limitPages && m_pstate[currTail].timerExpired)
            {
                if (!(ctrlFlags & CtrlOverlay::CTRLQ_CLAIM_MASK))
                {
                    if (nextTailRef == currTailRef)
                    {
                        // Don't return incomplete pages that have not been claimed;
                        // just discard them immediatly.
                        TRACE("free-unclaimed pg<%u-%u> due to [timer-expired and %u < %u of %u]",
                            currTail, currTail + ctrlPageCount - 1,
                            availPages, limitPages, pageCount());
                        discard = true;
                    }
                }
                else if (!m_pstate[currTail].retrieved)
                {
                    TRACE("incomplete pg<%u-%u> due to [timer-expired and %u < %u of %u]",
                        currTail, currTail + ctrlPageCount - 1,
                        availPages, limitPages, pageCount());

                    if (currTail != pstateSkipUpdateIdx)
                    {
                        m_pstate[pstateSkipUpdateIdx].skipCount += m_pstate[currTail].skipCount;
                        TRACE_PSTATE(pstateSkipUpdateRef, "->");
                    }
                    return walkEnd(item, currTailRef, ctrlPageCount, ctrlSize);
                }
            }
            else
            {
                TRACE("skip-%s pg<%u-%u> due to [timer-%s or %u >= %u of %u]",
                    m_pstate[currTail].retrieved ? "retrieved" : "incomplete",
                    currTail, currTail + ctrlPageCount - 1,
                    m_pstate[currTail].timerExpired ? "expired" : "running",
                    availPages, limitPages, pageCount());
            }
            if (nextTailRef != currTailRef)
            {
                pstateSkipUpdateIdx = currTail;
                pstateSkipUpdateRef = currTailRef;
            }

        }

        if (discard)
        {
            uint32_t advanceTailRef = c->queueRefIncrement(currTailRef, ctrlPageCount);
            uint32_t newCtrl = 0;
            if (ctrlFlags != 0)
            {
                // If the flags were zero the new control field is also zero.  If they are
                // non-zero it was valid and we need to adjust it with new flags.
                newCtrl = ctrlTail & ~CtrlOverlay::CTRLQ_FLAGS_MASK;
                if (ctrlFlags == (CtrlOverlay::CTRLQ_COMMIT_MASK | CtrlOverlay::CTRLQ_DISCARD_MASK))
                {
                    // Waste
                    newCtrl |= CtrlOverlay::CTRLQ_DISCARD_MASK;
                }
                else
                {
                    // Item
                    newCtrl |= CtrlOverlay::CTRLQ_COMMIT_MASK;
                }
            }
            testPoint(WalkBeforeWriteCtrl);
            if (Atomic::cmpXchg(&c->ctrlq[currTail], newCtrl, ctrlTail) == ctrlTail)
            {
                // Discard successful - there was no attempt to update the entry in parallel.
                // We can now move the tail reference.
                testPoint(WalkBeforeWriteTailRef);
                Atomic::write(&c->tailRef, advanceTailRef);
                Atomic::increment(&c->freeCounter);

                // Move the skip count to the next pstate entry if any remains then clear the
                // current pstate entry.
                pstateSkipUpdateIdx = c->queueRefToIndex(advanceTailRef);
                pstateSkipUpdateRef = currTailRef;
                if (m_pstate[currTail].skipCount > ctrlPageCount)
                {
                    m_pstate[pstateSkipUpdateIdx].skipCount = m_pstate[currTail].skipCount - ctrlPageCount;
                    TRACE_PSTATE(pstateSkipUpdateRef, "->");
                }
                memset(&m_pstate[currTail], 0, sizeof(m_pstate[currTail]) * ctrlPageCount);
                TRACE_PSTATE(currTailRef, "->");
                currTailRef = advanceTailRef;
                nextTailRef = advanceTailRef;
            }
        }
        else if (item != NULL)
        {
            // Just advance the current tail reference to reach the next frame.
            // We cannot free items anymore.
#ifdef ADVANCE_WITH_PSTATE
            uint32_t advance = m_pstate[currTail].skipCount;
            if (advance == 0)
            {
                advance = 1;
            }
            currTailRef = c->queueRefIncrement(currTailRef, advance);
#else
            currTailRef = c->queueRefIncrement(currTailRef, ctrlPageCount);
#endif
        }
        else
        {
            // Cannot discard any further and no log item requested; 
            // just return.
            TRACE_EXIT();
            return false;
        }
    }
    TRACE_EXIT();
    return false;
}

//------------------------------------------------------------------------------
bool AQReader::walkEnd(AQItem *item, uint32_t ref,
                              unsigned int advance, size_t memSize)
{
    // If we were not requested to actually process the data return false.
    if (item == NULL)
    {
        return false;
    }

    uint32_t pageNum = m_ctrl->queueRefToIndex(ref);

    m_pstate[pageNum].retrieved = 1;
    TRACE_PSTATE(ref, "->");

    item->m_mem = m_ctrl->pageToMem(pageNum);
    item->m_memSize = memSize;
    item->m_ctrl = m_ctrl->ctrlq[pageNum];
    item->m_quid = ref & AQItem::QUEUE_IDENTIFIER_MASK;

    // Get the link ID if link IDs are enabled.
    if (m_ctrl->options & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER)
    {
        pageNum += m_ctrl->pageCount;

        item->m_lkid = m_ctrl->ctrlq[pageNum];
    }
    else
    {
        item->m_lkid = AQItem::QUEUE_IDENTIFIER_INVALID;
    }

    // Calculate and store the CRC result if CRC's are enabled.
    if (m_ctrl->options & OPTION_CRC32)
    {
        pageNum += m_ctrl->pageCount;

        uint32_t calcCrc = CalculateItemCrc32(*item, m_ctrl->options);
        item->m_checksumValid = m_ctrl->ctrlq[pageNum] == calcCrc;
        if (item->m_checksumValid)
        {
            TRACE("crc[%08X]", calcCrc);
        }
        else
        {
            TRACE_1ITEMDATA(m_ctrl, item, "crc[%08X] ERROR expect[%08X]", 
                calcCrc, m_ctrl->ctrlq[pageNum]);
        }
    }
    else
    {
        item->m_checksumValid = true;
    }

    return true;
}




//=============================== End of File ==================================
