#ifndef AQLOGFORMATTER_H
#define AQLOGFORMATTER_H
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

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogRecord;
class AQLogStringBuilder;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Defines the base class for all log formatters.  A log formatter is used to 
 * take an AQLogRecord and convert it into a string that is to be send to an
 * AQLogHandler.  Specific implementations of the AQLogFormatter support
 * different output formats
 */
class AQLogFormatter
{
protected:

    /**
     * Constructs a new log formatter.
     */
    AQLogFormatter(void) { }

private:
    // Duplication and assignment are not supported.
    AQLogFormatter(const AQLogFormatter& other);
    AQLogFormatter& operator=(const AQLogFormatter& other);

public:

    /**
     * Destroys this log formatter instance.
     */
    virtual ~AQLogFormatter(void) { }

    /**
     * Formats the a log record into a string builder.
     *
     * @param rec The record to format.  The record is guaranteed to be valid.
     * @param sb The string builder into which the record is to be formatted.
     * It is guaranteed that so long as the string builder is in scope, all 
     * strings returned by the record will also remain in scope.  This means
     * that it is possible to directy insert the pointers taken from rec into
     * sb.
     */
    virtual void format(const AQLogRecord& rec, AQLogStringBuilder& sb) const = 0;

};




#endif
//=============================== End of File ==================================
