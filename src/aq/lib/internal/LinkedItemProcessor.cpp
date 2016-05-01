//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "LinkedItemProcessor.h"

#include "CtrlOverlay.h"

using namespace aq;
using namespace std;




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
LinkedItemProcessor::LinkedItemProcessor(void)
{
}

//------------------------------------------------------------------------------
LinkedItemProcessor::~LinkedItemProcessor(void)
{
    reset();
}

//------------------------------------------------------------------------------
void LinkedItemProcessor::reset(void)
{
    for (map<uint32_t, AQItem *>::iterator it = m_incomplete.begin(); 
         it != m_incomplete.end(); ++it)
    {
        delete it->second;
    }
    m_incomplete.clear();

    for (map<uint32_t, AQItem *>::iterator it = m_outOfOrder.begin();
    it != m_outOfOrder.end(); ++it)
    {
        delete it->second;
    }
    m_outOfOrder.clear();
}

//------------------------------------------------------------------------------
LinkedItemProcessor::Outcome LinkedItemProcessor::nextItem(AQItem& item)
{
    uint32_t lkid = item.linkIdentifier();
    uint32_t nextQuid = lkid & AQItem::QUEUE_IDENTIFIER_MASK;

    if (lkid & AQItem::LINK_IDENTIFIER_FIRST)
    {
        // This is the first item in a linked list.  If it is the last item,
        // or we cannot trust the link identifier (due to it not being committed
        // or the checksum failing) then just return exactly this item.
        if ((lkid & AQItem::LINK_IDENTIFIER_LAST)
            || !item.isCommitted()
            || !item.isChecksumValid())
        {
            // Only update the size if it reduces the size otherwise we could
            // overflow the buffer.
            if (item.isCommitted() && nextQuid > 0 && nextQuid < item.m_memSize)
            {
                item.m_memSize = nextQuid;
            }
            return PRODUCED;
        }
        else
        {
            // First item in a set - create a new object.
            return processOutOfOrder(nextQuid, new AQItem(item), item);
        }
    }

    // The link ID does no include 'first'; hence we must look it up in the map.
    map<uint32_t, AQItem *>::iterator it = m_incomplete.find(item.queueIdentifier());
    if (it == m_incomplete.end())
    {
        if (item.isCommitted())
        {
            // Unable to find this item in the list; must have retrieved it out of
            // order.
            m_outOfOrder[item.queueIdentifier()] = new AQItem(item);
            return CONSUMED;
        }
        else
        {
            return DISCARD;
        }
    }

    // Found an existing item, link the item to the new item.
    // Get the end of the list.
    AQItem *first = it->second;
    AQItem *newItem = new AQItem(item);
    appendItem(first, newItem);
    m_incomplete.erase(it);

    // If this is the last item, or it contains errors, then return it as
    // a new item.
    if ((lkid & AQItem::LINK_IDENTIFIER_LAST)
        || !item.isCommitted()
        || !item.isChecksumValid())
    {
        return produceCompleteItem(first, newItem, item);
    }

    // End of item not yet found; lets add it to the list.
    return processOutOfOrder(nextQuid, first, item);
}

//------------------------------------------------------------------------------
LinkedItemProcessor::Outcome LinkedItemProcessor::processOutOfOrder(
    uint32_t nextQuid, AQItem *first, AQItem& item)
{
    map<uint32_t, AQItem *>::iterator it;
    while ((it = m_outOfOrder.find(nextQuid)) != m_outOfOrder.end())
    {
        AQItem *next = it->second;
        m_outOfOrder.erase(it);
        
        appendItem(first, next);
        uint32_t lkid = next->linkIdentifier();
        if ((lkid & AQItem::LINK_IDENTIFIER_LAST)
            || !next->isCommitted()
            || !next->isChecksumValid())
        {
            return produceCompleteItem(first, next, item);
        }

        nextQuid = lkid & AQItem::QUEUE_IDENTIFIER_MASK;
    }

    m_incomplete[nextQuid] = first;
    return CONSUMED;
}

//------------------------------------------------------------------------------
void LinkedItemProcessor::appendItem(AQItem *first, AQItem *newItem)
{
    AQItem *last = first->m_prev;
    newItem->m_first = first;

    newItem->m_prev = last;
    last->m_next = newItem;
    first->m_prev = newItem;
}

//------------------------------------------------------------------------------
LinkedItemProcessor::Outcome LinkedItemProcessor::produceCompleteItem(
    AQItem *first, AQItem *last, AQItem& item)
{
    uint32_t size = last->linkIdentifier() & AQItem::QUEUE_IDENTIFIER_MASK;

    // Only update the size if it reduces the size otherwise we could
    // overflow the buffer.
    if (last->isCommitted() && size > 0 && size < last->m_memSize)
    {
        last->m_memSize = size;
    }

    // Move the data into 'item'.
    item.m_checksumValid = first->m_checksumValid;
    item.m_ctrl = first->m_ctrl;
    item.m_lkid = first->m_lkid;
    item.m_memSize = first->m_memSize;
    item.m_mem = first->m_mem;
    item.m_quid = first->m_quid;
    item.m_prev = first->m_prev;
    item.m_next = first->m_next;
    if (item.m_next != NULL)
    {
        item.m_next->m_prev = &item;
    }
    for (AQItem *curr = &item; curr != NULL; curr = curr->m_next)
    {
        curr->m_first = &item;

        // Populate the checksum validation and commit results.
        curr->m_checksumValid = last->m_checksumValid;
        if (!last->isCommitted())
        {
            curr->m_ctrl &= ~CtrlOverlay::CTRLQ_COMMIT_MASK;
        }
    }

    // Delete the previous head.
    first->m_next = NULL;
    first->m_prev = first;
    delete first;
    return PRODUCED;
}


//=============================== End of File ==================================
