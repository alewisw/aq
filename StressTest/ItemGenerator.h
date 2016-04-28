#ifndef ITEMGENERATOR_H
#define ITEMGENERATOR_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Prng.h"

#include <stddef.h>
#include <stdint.h>
#include <vector>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Minimum item length; format is:
//
// LLLLLLLL,TTTTT,CCCCCCCCCCCC:
//
// Where:
//  L is the record length in decimal.
//  T is the thread ID.
//  C is the item counter.
//
// Records are always nul-terminated.
#define RECORD_LEN_CHARS                8
#define RECORD_THREAD_ID_CHARS          5
#define RECORD_COUNT_CHARS              12
#define RECORD_MIN_LEN                  (RECORD_LEN_CHARS + 1 + RECORD_THREAD_ID_CHARS + 1 + RECORD_COUNT_CHARS + 1 + 1)
#define RECORD_LEN_FORMAT               "%08u"
#define RECORD_FORMAT                   RECORD_LEN_FORMAT ",%05d,%012llu:"




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates a random number generator used to create a fixed sequence of 
// items.  The items are configured based on a set of configuration parameters
// passed into the constructor.
class ItemGenerator
{
public:

    // Constructs a new item generator which seeds based on the thread number.
    ItemGenerator(int threadNum, size_t pageSize, const std::vector<unsigned int>& itemPages);

    // Creats an exact copy of this item generator including its current PRNG
    // state.
    ItemGenerator(const ItemGenerator& other);

    // Assigns the value of this item generator to exactly match another.
    ItemGenerator& operator=(const ItemGenerator& other);

    // Destroys this item generator.
    virtual ~ItemGenerator(void);

private:

    // The thread number for this generator.
    int m_threadNum;

    // The page size for the queue.
    size_t m_pageSize;

    // The set of page sizes that are availableSize for item generation.
    std::vector<unsigned int> m_itemPages;

    // The number of items produced so far.
    unsigned long long m_count;

    // The PRNG used for this item generator.
    Prng m_prng;

    // The maximum size for a item.
    size_t m_itemMaxSize;

    // The item memory used by this generator.
    unsigned char *m_item;

    // The size of the random buffer.
    size_t m_randBufSize;

    // The random data buffer used by this generator.
    unsigned char *m_randBuf;

public:

    // Returns the size of pages in the queue.
    size_t pageSize(void) const { return m_pageSize; }

    // Returns the current packet counter for this generator.
    unsigned long long count(void) const { return m_count; }

    // Generates and returns a pointer to the next item.  The item size
    // is placed in 'memSize'.
    const unsigned char *next(size_t& memSize, uint32_t& linkId);

    // Gets and returns a pointer to the current item.  The item size
    // is placed in 'memSize'.
    const unsigned char *get(size_t& memSize, uint32_t& linkId) const;

    // Moves back to the previous state.  This can only be called once after
    // 'next' is called.
    void next(void);

};



#endif
//=============================== End of File ==================================
