#ifndef DEFAULTFORMATTER_H
#define DEFAULTFORMATTER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogFormatter.h"




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

// Defines the default log formatter that is used when no log formatter is 
// specified for a given log handler.  The default log formatter is designed
// to produce human-readable text output.
//
// There are no configurable options for the default log formatter and for this
// reason it is not possible to directy construct run; rather simply do not set
// the log formatter for a particular handler and it will fall back to the
// default log formtter.
namespace aqlog { class DefaultFormatter : public AQLogFormatter
{
public:

    // Constructs a default formatter.
    DefaultFormatter(void);

private:
    // Duplication and assignment are not supported.
    DefaultFormatter& operator=(const DefaultFormatter& other);

public:

    // Destroys this log formatter instance.
    virtual ~DefaultFormatter(void);

    // Formats the a log record into a string builder.
    virtual void format(const AQLogRecord& rec, AQLogStringBuilder& sb) const;

};}




#endif
//=============================== End of File ==================================
