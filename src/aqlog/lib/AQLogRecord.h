#ifndef AQLOGRECORD_H
#define AQLOGRECORD_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLog.h"

#include "AQLogStringBuilder.h"

#include "AQItem.h"

#include <vector>

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogHandler;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Encapsuates a single log record retrieved from the log queue and decoded into
 * usable fields.
 */
class AQLogRecord
{
public:

    // Used to overlay raw item memory to obtain the fields for this log record
    // that are fixed in length.
    struct Overlay
    {
        // The number of nanoseconds since the Unix epoch (1 January 1970) 
        // indicating when this log record was generated.
        uint64_t timestampNs;

        // The process identifier for this log record.
        uint32_t processId;

        // The thread identifier for this log record.
        uint32_t threadId;

        // The number of data bytes appended to this log record.
        uint32_t dataSize;

        // If set to '1' then the strings in this log record have been truncated.
        uint32_t truncatedStr : 1;

        // If set to '1' then at least one message has been dropped from the log
        // due to an out-of-space condition before this one was published.
        uint32_t dropped : 1;

        // Reserved for future use.
        uint32_t reservedFlag : 1;

        // If set to '1' then the data in this log record has been truncated.
        uint32_t truncatedData : 1;

        // The log level for this record.
        uint32_t logLevel : 4;

        // The line number where this log record was generated.
        uint32_t lineNumber : 24;

        // The string data region for this log record.  The string data region
        // is constructed as follows:
        //  {data} - {dataSize} bytes
        //  "processName" '\0'
        //  "componentId" '\0'
        //  "tagId"       '\0'  (only if hasTag is '1')
        //  "file"        '\0'
        //  "function"    '\0'
        //  "message"     '\0'
        char strData[1];

    };

public:

    // Create a new empty AQLogRecord object.
    AQLogRecord(void);
    
    // Creates a new record.  This constructor is used for unit testing only,
    // it just sets the passed level and IDs.
    AQLogRecord(AQLogLevel_t level, const char *componentId,
        const char *tagId, const char *file);

    // Destroys this filter.
    ~AQLogRecord(void);

private:
    // Duplication and asignment are not supported.
    AQLogRecord(const AQLogRecord& other);
    AQLogRecord& operator=(const AQLogRecord& other);

public:

    // Gets the item that holds the data for this record.
    AQItem& aqItem(void) { return m_item; }

    // Possible outcomes from the populate call.
    enum PopulateOutcome
    {
        // Population succeeded.
        POPULATE_SUCCESS = 0,

        // Population failed due to this being an uncommitted record.
        POPULATE_ERROR_UNCOMMITTED,

        // Population failed due to this record having a checksum error.
        POPULATE_ERROR_CHECKSUM,

        // Population failed due to this record having a size less than 
        // the overlay size.
        POPULATE_ERROR_TRUNCATED_HEADER,

        // Population failed due to this record having a size less than 
        // the size required for the non-message fields.
        POPULATE_ERROR_TRUNCATED_DATA,

        // Population failed due to this record not containing enough bytes
        // for the component ID.
        POPULATE_ERROR_TRUNCATED_COMPONENT_ID,

        // Population failed due to this record not containing enough bytes
        // for the tag ID.
        POPULATE_ERROR_TRUNCATED_TAG_ID,

        // Population failed due to this record not containing enough bytes
        // for the source file name.
        POPULATE_ERROR_TRUNCATED_FILE,

        // Population failed due to this record not containing enough bytes
        // for the process name.
        POPULATE_ERROR_TRUNCATED_PROCESS_NAME,

        // Population failed due to this record not containing enough bytes
        // for the function name.
        POPULATE_ERROR_TRUNCATED_FUNCTION,

        // The number of possible outcomes for population.
        POPULATE_OUTCOME_COUNT,
        
    };

    // Populates the fields of this record with the current content of the AQ item.
    PopulateOutcome populate(void);

    // The monotonic clock time when this item was processed in milliseconds.
    uint32_t processTimeMs(void) const { return m_processTimeMs; }

    // Gets the identifier used at a particular filter tier 'idx'.
    const char *tierId(size_t idx) const { return m_tierId[idx]; }

    // Returns true if this record also reports that a previous record has been
    // dropped in its entirity due to an out of space condition.
    bool isReportingOutOfSpaceDrop(void) const { return !!m_overlay->dropped; }

private:

    // The log level for this record.
    AQLogLevel_t m_level;

    // The tier look-up for the log record at each look-up level.
    const char *m_tierId[AQLOG_LOOKUP_TIER_COUNT + AQLOG_EXTRA_TIER_COUNT];

    // The item holding the data for this record.
    AQItem m_item;

    // The overlay structure for this record.  Points into m_item.
    const Overlay *m_overlay;

    // The monotonic clock time when this item was processed in milliseconds.
    uint32_t m_processTimeMs;

    // The message for this record.
    AQLogStringBuilder m_message;

public:

    /**
     * Obtains the timestamp when this record was generated.  The timestamp is
     * measured in nanoseconds since 1 January 1970, the start of the Unix epoch.
     *
     * @return The timestamp in nanoseconds.
     */
    uint64_t timestampNs(void) const { return m_overlay->timestampNs; }

    /**
     * Obtains the logging level for this record.
     *
     * @return The logging level.
     */
    AQLogLevel_t level(void) const { return m_level; }

    /**
     * Obtains the number of data bytes associated with this record.
     *
     * @return The number of data bytes or 0 if there is no data associated with this record.
     */
    size_t dataSize(void) const { return m_overlay->dataSize; }

    /**
     * Determines if the data provided by this log record has been truncated.
     * Data truncation occurs when it is not possible to allocate enough space
     * in the queue to contain all the data provided when the record was logged.
     *
     * @return True if the data is truncated, false if the data is complete.
     */
    bool isDataTruncated(void) const { return !!m_overlay->truncatedData; }

    /**
     * Obtains a pointer to the data associated with this record.  This pointer is
     * only valid if dataSize() returned a non-zero value.
     *
     * @return A pointer to the data associated with this record.  If dataSize() is
     * 0 the return value is undefined.
     */
    const void *data(void) const { return &m_overlay->strData[0]; }

    /**
     * Obtains the name of the process that generated this log record.
     *
     * @return The process name string.
     */
    const char *processName(void) const { return m_tierId[AQLOG_EXTRA_TIER_PROCESS_NAME]; }

    /**
    * Obtains the identifier of the process that generated this log record.
    *
    * @return The process identifier as an unsigned 32-bit integer.
    */
    uint32_t processId(void) const { return m_overlay->processId; }

    /**
     * Obtains the identifier of the thread that generated this log record.
     *
     * @return The thread identifier as an unsigned 32-bit integer.
     */
    uint32_t threadId(void) const { return m_overlay->threadId; }

    /**
     * Obtains the component identifier for this record.
     *
     * @return The component identifier string.
     */
    const char *componentId(void) const { return m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID]; }

    /**
     * Obtains the tag identifier for this record.
     *
     * @return The tag identifier string.  This is the empty string if no tag
     * identifier was specified when the record was logged.
     */
    const char *tagId(void) const { return m_tierId[AQLOG_LOOKUP_TIER_TAGID]; }

    /**
     * Obtains the name of the file where this record was generated.
     *
     * @return The file name string.
     */
    const char *file(void) const { return m_tierId[AQLOG_LOOKUP_TIER_FILE]; }

    /**
     * Obtains the line number in the file (given by file()) where this record
     * was generated.
     *
     * @return The line number.
     */
    uint32_t lineNumber(void) const { return m_overlay->lineNumber; }

    /**
     * Obtains the name of the function that generated this record.
     *
     * @return The function name string.
     */
    const char *function(void) const { return m_tierId[AQLOG_EXTRA_TIER_FUNCTION]; }

    /**
     * Determines if the message provided by this log record has been truncated.
     * Message truncation occurs when it is not possible to allocate enough space
     * in the queue to contain the entire message that was supplied with the
     * record was logged.
     *
     * @return True if the message is truncated, false if the message is complete.
     */
    bool isMessageTruncated(void) const { return !!m_overlay->truncatedStr; }

    /**
     * Obtains the message for this log record.
     *
     * @return The message for this log record.
     */
    const AQLogStringBuilder& message(void) const { return m_message; }

};




#endif
//=============================== End of File ==================================
