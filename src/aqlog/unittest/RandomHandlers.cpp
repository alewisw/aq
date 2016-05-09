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

#include "TestHandler.h"
#include "HashData.h"




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
{
    for (size_t i = 0; i < n; ++i)
    {
        TestHandler *h = new TestHandler;
        m_handlers.push_back(h);

        size_t nFilters = (rand() % 5) + 1;
        for (size_t j = 0; j < nFilters; ++j)
        {
            AQLogLevel_t level = (AQLogLevel_t)(rand() % (maxLevel + 1));
            const char *componentId;
            switch (rand() % 4)
            {
            case 0:
                componentId = "";
                break;

            default:
                componentId = HashIdxTable_g[rand() % HASHIDX_TABLE_COUNT];
                break;
            }
            const char *tagId;
            switch (rand() % 4)
            {
            case 0:
                tagId = "";
                break;

            default:
                tagId = HashIdxTable_g[rand() % HASHIDX_TABLE_COUNT];
                break;
            }
            const char *fileId;
            switch (rand() % 4)
            {
            case 0:
                fileId = "";
                break;

            default:
                fileId = HashIdxTable_g[rand() % HASHIDX_TABLE_COUNT];
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
    for (size_t i = 0; i < m_handlers.size(); ++i)
    {
        delete m_handlers[i];
    }
}



//=============================== End of File ==================================
