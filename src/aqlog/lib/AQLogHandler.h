#ifndef AQLOGHANDLER_H
#define AQLOGHANDLER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogFilter.h"

#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogRecord;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Log handlers are used to process log records writing them out to their 
 * ultimate destination.  Each log handler contains a set of filters, and if a 
 * record matches any one of those filters then it is passed to the log handler
 * for proessing.
 *
 * This is an abstract class; concrete implementations override the handle() 
 * method to actually process the log record.
 */
class AQLogHandler
{
protected:

    /**
     * Constructs a new log handler.  The log handler has no associated
     * filters.
     */
    AQLogHandler(void);

    /**
     * Destroys this log handler.
     */
    virtual ~AQLogHandler(void);

public:

    /**
     * Adds a filter to this handler.  Any log record that matches the 
     * filter is passed to the handle() function.  All the filter conditions
     * must match on the log record in order for it to be handled.
     *
     * @param level The log level for the filter.  Log records must have a
     * level equal to or less than this value to be handled.  Any
     * log record with a level higher than this number is ignored by this
     * handler.  Note that the lowest log level corresponds to the highest 
     * severity level.
     * @param componentId The component required for this filter.
     *   - If this string is non-empty then each log record must originate 
     *     from this component.  Log records from any other component are 
     *     ignored.
     *   - If this is the empty string then no filtering is applied to the 
     *     component.  All log records will be accepted regardless of their
     *     component state.
     * @param tagId The tag required for this filter.
     *   - If this string is non-empty then each log record must have exactly
     *     the tag specified in this parameter.  Log records with any other
     *     tag, or no tag at all, are ignored.
     *   - If this is the empty string then no filtering is applied to the
     *     tag.  All log records will be accepted regardless of their tag
     *     value.
     * @param file The filename required for this filter.  Filenames never
     *     include the file directory path, only the filename itself.
     *   - If this string is non-empty then each log record must originate
     *     from a file with this name.  Log records from any other file are
     *     ignored.
     *   - If this is the empty string then no filtering is applied base on
     *     the originating file name.  All log records will be accepted 
     *     regardless of their file name.
     *  It is recommended that when specifying a file-based filter that either
     *  a tag or a component is also provided.  Only specifying a file can 
     *  lead to poor performance as other third party components could contain
     *  files with the same name, and moreover many files may hash to the same
     *  position in the log level hash table leading to records being generated
     *  for other components that are never actually handled by a log handler.
     */
    void addFilter(AQLogLevel_t level, const std::string& componentId = "", 
        const std::string& tagId = "", const std::string& file = "");

    /**
     * Called whenever a log record is received that needs to be handled by 
     * this log handler.
     *
     * @param rec The record to handle.
     */
    virtual void handle(const AQLogRecord& rec) = 0;

private:

    // The filters for this handler.
    std::vector<AQLogFilter> m_filters;

public:

    // The array of the filters that cause records to be stored in this handler.
    const std::vector<AQLogFilter>& filters(void) const { return m_filters; }

};




#endif
//=============================== End of File ==================================
