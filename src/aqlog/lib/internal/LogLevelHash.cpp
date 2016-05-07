//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "LogLevelHash.h"

#include "AQLogFilter.h"
#include "AQLogHandler.h"
#include "AQLogRecord.h"

#include <set>

using namespace std;

namespace aqlog {




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

// Look-up table that maps the tier to the number of bits for that tier.
static const uint32_t TierBits[AQLOG_LOOKUP_TIER_COUNT] =
{
    AQLOG_TIER_0_BITS,
    AQLOG_TIER_1_BITS,
    AQLOG_TIER_2_BITS
};

// Look-up table that maps the tier to the bit number for that tier.
static const uint32_t TierBitnum[AQLOG_LOOKUP_TIER_COUNT] =
{
    AQLOG_TIER_0_BITNUM,
    AQLOG_TIER_1_BITNUM,
    AQLOG_TIER_2_BITNUM
};

// Look-up table that maps the tier to the number of bits for that tier.
static const uint32_t TierSize[AQLOG_LOOKUP_TIER_COUNT] =
{
    AQLOG_TIER_0_BITS + AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM,
    AQLOG_TIER_1_BITS + AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM,
    AQLOG_TIER_2_BITS - AQLOG_HASH_INDEX_WORD_BITNUM,
};




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
LogLevelHash::LogLevelHash(uint32_t *hashMem, HashFunction_fn hashFn)
    : m_freezeState(FREEZE_OFF)
    , m_hashMem(hashMem)
    , m_hashFn(hashFn)
{
    if (hashMem != NULL)
    {
        memset(hashMem, 0, sizeof(uint32_t) * AQLOG_HASH_TABLE_WORDS);
    }
}

//------------------------------------------------------------------------------
LogLevelHash::~LogLevelHash(void)
{
}

//------------------------------------------------------------------------------
void LogLevelHash::freezeHash(void)
{
    if (m_freezeState != FREEZE_REBUILD)
    {
        m_freezeState = FREEZE_ON;
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::unfreezeHash(void)
{
    if (m_freezeState == FREEZE_REBUILD)
    {
        rebuildHash();
    }
    m_freezeState = FREEZE_OFF;
}

//------------------------------------------------------------------------------
void LogLevelHash::addHandler(AQLogHandler *handler)
{
    const std::vector<AQLogFilter>& filters = handler->filters();

    for (size_t i = 0; i < filters.size(); ++i)
    {
        addFilter(&filters[i]);
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::removeHandler(AQLogHandler *handler)
{
    const std::vector<AQLogFilter>& filters = handler->filters();

    for (size_t i = 0; i < filters.size(); ++i)
    {
        removeFilter(&filters[i]);
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::addFilter(const AQLogFilter *filter)
{
    FilterMap *fm = &m_filters;
    for (size_t i = 0; i < filter->keyCount(); ++i)
    {
        const string& key = filter->key(i);

        map<string, FilterMap>::iterator it = fm->m_children.find(key);
        if (it == fm->m_children.end())
        {
            fm = &fm->m_children[key];
        }
        else
        {
            fm = &it->second;
        }
    }

    // Linear search for the correct position in the handler list.  Highest
    // number first.
    list<const AQLogFilter *>::iterator it = fm->m_handlers.begin();
    while (it != fm->m_handlers.end() && (*it)->level() > filter->level())
    {
        it++;
    }

    // Determine if we need to update the shared hash table with the new value.
    // We only need to make a change if:
    //  - we are inserting at the end meaning we must have a level greater 
    //    than anything else in the queue.
    //  - the last item in the list has a filter level less than the new 
    //    level.
    if (m_hashMem != NULL && (it == fm->m_handlers.end() || fm->m_handlers.back()->level() < filter->level()))
    {
        if (m_freezeState == FREEZE_OFF)
        {
            populateHash(m_hashMem, *filter, 0, 0);
        }
        else
        {
            m_freezeState = FREEZE_REBUILD;
        }
    }

    // Insert the filter.
    fm->m_handlers.insert(it, filter);
}

//------------------------------------------------------------------------------
void LogLevelHash::removeFilter(const AQLogFilter *filter)
{
    FilterMap *fm = &m_filters;
    for (size_t i = 0; i < filter->keyCount(); ++i)
    {
        const string& key = filter->key(i);

        map<string, FilterMap>::iterator it = fm->m_children.find(key);
        if (it == fm->m_children.end())
        {
            return;
        }
        else
        {
            fm = &it->second;
        }
    }

    // Linear search for the filter until we know it cannot exist.  Determine if
    // it was the highest filter level in the list - if so we need to rebuild
    // the hash list.
    list<const AQLogFilter *>::iterator it = fm->m_handlers.begin();
    bool rebuild = it != fm->m_handlers.end() && (*it)->level() == filter->level();
    while (it != fm->m_handlers.end() && (*it)->level() >= filter->level())
    {
        const AQLogFilter *f = *it;
        if (f == filter)
        {
            fm->m_handlers.erase(it);
            break;
        }
        it++;
    }

    if (rebuild)
    {
        if (m_freezeState == FREEZE_OFF)
        {
            rebuildHash();
        }
        else
        {
            m_freezeState = FREEZE_REBUILD;
        }
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::handle(const AQLogRecord& rec)
{
    set<AQLogHandler *> handlers;

    // Recursivly populate all the handlers for this log record.
    populateHandlers(handlers, rec, m_filters, 0);

    // Execute all the handlers.
    for (set<AQLogHandler *>::iterator it = handlers.begin(); it != handlers.end(); ++it)
    {
        (*it)->handle(rec);
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::populateHandlers(std::set<AQLogHandler *>& handlers, 
    const AQLogRecord& rec, FilterMap& fm, size_t tier)
{
    // First populate with everything in this filter map.
    std::list<const AQLogFilter *>::iterator it = fm.m_handlers.begin();
    while (it != fm.m_handlers.end() && (*it)->level() >= rec.level())
    {
        handlers.insert(&(*it)->handler());
        it++;
    }

    if (tier < AQLOG_LOOKUP_TIER_COUNT)
    {
        // Tier specific handers - ones that match all strings for this tier then
        // the ones that only match a specified component.
        map<string, FilterMap>::iterator it = fm.m_children.find("");
        if (it != fm.m_children.end())
        {
            populateHandlers(handlers, rec, it->second, tier + 1);
        }
        it = fm.m_children.find(rec.tierId(tier));
        if (it != fm.m_children.end())
        {
            populateHandlers(handlers, rec, it->second, tier + 1);
        }
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::populateHash(uint32_t *hashMem, const AQLogFilter& filter, 
    uint32_t index, uint32_t tier)
{
    if (tier < AQLOG_LOOKUP_TIER_COUNT)
    {
        const std::string& t = filter.key(tier);
        uint32_t bits = TierBits[tier];

        if (t.size() == 0)
        {
            // This filter applies to all entries at this tier.
            for (uint32_t hash = 0; hash < (uint32_t)(1 << bits); ++hash)
            {
                populateHash(hashMem, filter, (index << bits) | hash, tier + 1);
            }
        }
        else
        {
            // This filter applies to a single entry.
            uint32_t hash = m_hashFn(((1 << bits) - 1), t.c_str(), t.size(), tier == AQLOG_LOOKUP_TIER_TAGID);
            populateHash(hashMem, filter, (index << bits) | hash, tier + 1);
        }
    }
    else
    {
        uint32_t word = index >> AQLOG_HASH_INDEX_WORD_BITNUM;
        uint32_t offset = (index & AQLOG_HASH_INDEX_LEVEL_MASK) << AQLOG_HASH_LEVEL_BITS_MUL_SHIFT;

        uint32_t v = hashMem[word];
        uint32_t currentLevel = (v >> offset) & AQLOG_HASH_LEVEL_MASK;
        if (filter.level() > (AQLogLevel_t)currentLevel)
        {
            uint32_t newLevel = v & ~(AQLOG_HASH_LEVEL_MASK << offset);
            newLevel |= (AQLOG_HASH_LEVEL_MASK & filter.level()) << offset;

            hashMem[word] = newLevel;
        }
    }
}

//------------------------------------------------------------------------------
void LogLevelHash::rebuildHash(void)
{
    uint32_t tmpHashMem[AQLOG_HASH_TABLE_WORDS];

    // Fill with the default value based on the top level.
    AQLogLevel_t level = (AQLogLevel_t)0;
    if (m_filters.m_handlers.size() > 0)
    {
        level = m_filters.m_handlers.front()->level();
    }
    memset(tmpHashMem, (level << AQLOG_HASH_LEVEL_BITS) | level, 
           AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));

    // Now repopulate with every filter.
    repopulateHash(tmpHashMem, m_filters);

    // Finally copy it over our current hash.
    memcpy(m_hashMem, tmpHashMem, AQLOG_HASH_TABLE_WORDS * sizeof(uint32_t));
}

//------------------------------------------------------------------------------
void LogLevelHash::repopulateHash(uint32_t *hashMem, FilterMap& fm)
{
    for (map<string, FilterMap>::iterator it = fm.m_children.begin(); it != fm.m_children.end(); ++it)
    {
        FilterMap& cfm = it->second;
        if (cfm.m_handlers.size() > 0)
        {
            populateHash(hashMem, *cfm.m_handlers.front(), 0, 0);
        }
        repopulateHash(hashMem, cfm);
    }
}



}
//=============================== End of File ==================================
