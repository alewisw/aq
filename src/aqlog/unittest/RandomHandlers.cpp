//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"

#include "RandomHandlers.h"
#include "DataSets.h"
#include "TestHandler.h"

#include "Timer.h"

#include "Prng.h"

using namespace aqosa;




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
RandomHandlers::RandomHandlers(LogLevelHash& hash, AQLogLevel_t maxLevel, size_t n)
    : m_hash(hash)
{
    generate(Timer::start(), hash, maxLevel, n);
}

//------------------------------------------------------------------------------
RandomHandlers::RandomHandlers(uint32_t seed, LogLevelHash& hash,
    AQLogLevel_t maxLevel, size_t n)
    : m_hash(hash)
{
    generate(seed, hash, maxLevel, n);
}

//------------------------------------------------------------------------------
void RandomHandlers::generate(uint32_t seed, LogLevelHash& hash,
    AQLogLevel_t maxLevel, size_t n)
{
    Prng prng(seed);

    for (size_t i = 0; i < n; ++i)
    {
        TestHandler *h = new TestHandler;
        m_handlers.push_back(h);

        size_t nFilters = (prng.next() % 5) + 1;
        for (size_t j = 0; j < nFilters; ++j)
        {
            AQLogLevel_t level = (AQLogLevel_t)(prng.next() % (maxLevel + 1));
            const char *componentId;
            switch (prng.next() % 4)
            {
            case 0:
                componentId = "";
                break;

            default:
                componentId = HashIdxTable_g[prng.next() % HASHIDX_TABLE_COUNT];
                break;
            }
            const char *tagId;
            switch (prng.next() % 4)
            {
            case 0:
                tagId = "";
                break;

            default:
                tagId = HashIdxTable_g[prng.next() % HASHIDX_TABLE_COUNT];
                break;
            }
            const char *fileId;
            switch (prng.next() % 4)
            {
            case 0:
                fileId = "";
                break;

            default:
                fileId = HashIdxTable_g[prng.next() % HASHIDX_TABLE_COUNT];
                break;
            }
            h->addFilter(level, componentId, tagId, fileId);
        }

        hash.addHandler(h);
    }
}

//------------------------------------------------------------------------------
RandomHandlers::~RandomHandlers(void)
{
    m_hash.freezeHash();
    for (size_t i = 0; i < m_handlers.size(); ++i)
    {
        m_hash.removeHandler(m_handlers[i]);
        delete m_handlers[i];
    }
    m_hash.unfreezeHash();
}



//=============================== End of File ==================================
