#ifndef QUEUEPROVIDER_H
#define QUEUEPROVIDER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <string>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class IAQReader;
class IAQWriter;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines the interface implemented by all queue providers - a queue provider
// must give the before()/beforeIteration()/after() implementations along
// with access to the construction of queue objects.
class IQueueProvider
{
protected:

    // Stubs for this inteface.
    IQueueProvider(void) { }
    virtual ~IQueueProvider(void) { }

public:

    // Returns the one reader object for the queue.
    virtual IAQReader& reader(void) = 0;

    // Returns the one writer object for the queue.
    virtual IAQWriter& writer(void) = 0;

    // Returns the number of pages that can be used at the same time from this provider.
    virtual size_t usablePageCount(void) const = 0;

    // Returns the size of each page in this provider.
    virtual size_t pageSize(void) const = 0;

    // Called before the test is run to setup the test data, format the queues, and
    // so forth.
    virtual void before(void) = 0;

    // Called before each iteration of the test.  This must reset the queue to its
    // empty state.
    virtual void beforeIteration(void) = 0;

    // Called after the test is run to clean-up all objects allocated during the test.
    virtual void after(void) { };

    // Obtains a string description of this configuration.
    virtual std::string config(void) { return std::string(""); }

    // Obtains a string description of the queue state.
    virtual std::string results(void) { return std::string(""); }

};



#endif
//=============================== End of File ==================================
