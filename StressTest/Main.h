#ifndef MAIN_H
#define MAIN_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQItem.h"

#include <string>

using namespace std;


/*

86831 |   p8 | Q[  302->  146] |    commit:270 <-> item[0080012D]<cC-:Q8:301-303> L82 00008,000000000085:vkqqtmlazghqk


*/

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

// Parses the tag for the record given by 'item' for 'whom'.
// Returns the tag section of the record, storing the thread ID and count in the
// passed references.
extern std::string parseRecordTag(const char *whom, const AQItem *item,
                                  unsigned int &recLen, int &threadId, unsigned long long &count);

// Converts the passed item into a string.
extern std::string itemToString(const AQItem *item);

// Checks that the shared memory guard pages are valid and no overrun or
// underrun has occurred.
extern void assertShmGuard(void);

// Called whenever an assertion has failed with the specified message.
extern void assertFailed(const string &msg);

// Releases the passed reader item; called from a ConsumerChannel when it
// has finished with the item.
extern void freeReaderItem(AQItem *item);




#endif
//=============================== End of File ==================================
