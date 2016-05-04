#ifndef UTLOGLEVELHASH_H
#define UTLOGLEVELHASH_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogHandler.h"
#include "AQLogRecord.h"

#include "LogLevelHash.h"





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

// Defines the handler we use to run these tests.
class TestHandler : public AQLogHandler
{
public:

    // Constructs a new test handler.
    TestHandler::TestHandler(void) { }

    // Constructs a test handler with a filter as configured.
    TestHandler(AQLogLevel_t level, std::string tier1 = "",
        std::string tier2 = "", std::string tier3 = "")
    {
        addFilter(level, tier1, tier2, tier3);
    }

    // Destroys this test handler.
    virtual ~TestHandler(void)
    {
    }

    // Handles the passed log record.
    virtual void handle(const AQLogRecord& rec)
    {
        m_records.push_back(&rec);
    }

    // The log records handled.
    std::vector<const AQLogRecord *> m_records;

public:

    // Gets one of the records from this log level hash.
    const AQLogRecord *record(size_t idx) const
    {
        if (idx < m_records.size())
        {
            return m_records[idx];
        }
        else
        {
            return NULL;
        }
    }

};

// Used to add random handlers to a test in order to perturb the results.
class RandomHandlers
{
public:

    // Constructs a new random handler colection - there are 'n' handlers
    // added to 'hash'.
    RandomHandlers(LogLevelHash& hash, size_t n = 100)
    {
        for (size_t i = 0; i < n; ++i)
        {
            TestHandler *h = new TestHandler;
            m_handlers.push_back(h);

            size_t nFilters = (rand() % 5) + 1;
            for (size_t j = 0; j < nFilters; ++j)
            {
                AQLogLevel_t level = (AQLogLevel_t)(rand() % 8);
                const char *componentId;
                switch (rand() % 4)
                {
                default:    componentId = "";           break;
                case 0:     componentId = "comp_foo";   break;
                case 1:     componentId = "comp_bar";   break;
                case 2:     componentId = "comp_baz";   break;
                }
                const char *tagId;
                switch (rand() % 4)
                {
                default:    tagId = "";           break;
                case 0:     tagId = "tag_foo";    break;
                case 1:     tagId = "tag_bar";    break;
                case 2:     tagId = "tag_baz";    break;
                }
                const char *fileId;
                switch (rand() % 4)
                {
                default:    fileId = "";           break;
                case 0:     fileId = "file_foo";   break;
                case 1:     fileId = "file_bar";   break;
                case 2:     fileId = "file_baz";   break;
                }
                h->addFilter(level, componentId, tagId, fileId);
            }

            hash.addHandler(h);
        }
    }

    // Destroys the handlers.
    ~RandomHandlers(void)
    {
        for (size_t i = 0; i < m_handlers.size(); ++i)
        {
            delete m_handlers[i];
        }
    }

private:
    std::vector<TestHandler *> m_handlers;
};




#endif
//=============================== End of File ==================================
