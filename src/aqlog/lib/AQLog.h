#ifndef AQLOG_H
#define AQLOG_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

/** @file */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------


// Determine whether we should use the inline hash functions or the extern hash
// functions.
//
// The inline hash functions are structured so that the optimizer for the 
// compiler can resolve the hash down to a constant given a constant input string.
//
// However the compilers will only do this when optimization is enabled along
// with aggressive inlining.  As such detect the compiler type and its optimization
// mode.  If all is well use the inline functions - otherwise we have to fall back to
// the extern functions.
#if defined(_MSC_VER)
    // Compiler identified as Microsoft Visual C++.
    #define AQLOG_HASH_INLINE_ATTRIBUTE static __forceinline
    #define AQLOG_HASH_EXTERN_ATTRIBUTE

    #if defined(_DEBUG)
        // Debug build - in this mode the compiler will not inline functions.
        #undef AQLOG_HASH_USE_INLINE
    #else
        // Non-debug build - the compiler can be forced to inline.
        #define AQLOG_HASH_USE_INLINE
    #endif

#elif defined(__GNUC__)
    // Compiler identified as GCC.
    #define AQLOG_HASH_INLINE_ATTRIBUTE static inline __attribute__((always_inline)) __attribute__((flatten)) __attribute__((pure))
    #define AQLOG_HASH_EXTERN_ATTRIBUTE __attribute__((pure))

    #if defined(__NO_INLINE__)
        // Inlining has been disabled.
        #undef AQLOG_HASH_USE_INLINE
    #elif defined(__OPTIMIZE__)
        // Optimizations are turned on meaning we can support inlining.
        #define AQLOG_HASH_USE_INLINE
    #else
        // Optimizations are turned off hence the compiler will not inline.
        #undef AQLOG_HASH_USE_INLINE
    #endif
#else
    // Unidentified compiler - don't use the inline function, clear the extern attributes.
    #undef AQLOG_HASH_USE_INLINE
    #define AQLOG_HASH_INLINE_ATTRIBUTE static inline
    #define AQLOG_HASH_EXTERN_ATTRIBUTE
#endif

// Select the function used for inlining.
#if defined(AQLOG_HASH_USE_INLINE)
    #define AQLOG_HASHISLEVEL(__level_, __str1_, __str2_, __str3_)  AQLOG_HASHISLEVEL_INLINE(__level_, __str1_, __str2_, __str3_)
#else
    #define AQLOG_HASHISLEVEL(__level_, __str1_, __str2_, __str3_)  AQLOG_HASHISLEVEL_EXTERN(__level_, __str1_, __str2_, __str3_)
#endif

// The initial value for the hash.
#define AQLOG_HASH_INIT				    5381

// Performs a single step of the hash calculation hash[i] = fn(hash[i - 1], ch[i])
#define AQLOG_HASH_STEP(__hash_, __ch_) (((__hash_ << 5) + __hash_) + __ch_)

// The number of lookup tiers used in the AQLog filter.
#define AQLOG_LOOKUP_TIER_COMPONENTID   0
#define AQLOG_LOOKUP_TIER_TAGID         1
#define AQLOG_LOOKUP_TIER_FILE          2
#define AQLOG_LOOKUP_TIER_COUNT         3

#define AQLOG_EXTRA_TIER_PROCESS_NAME   3
#define AQLOG_EXTRA_TIER_FUNCTION       4
#define AQLOG_EXTRA_TIER_COUNT          2

// The number of bits per tier.
#define AQLOG_TIER_0_BITS               7
#define AQLOG_TIER_1_BITS               7
#define AQLOG_TIER_2_BITS               4

// The bit number number of bits per tier.
#define AQLOG_TIER_0_MASK               ((1 << AQLOG_TIER_0_BITS) - 1)
#define AQLOG_TIER_1_MASK               ((1 << AQLOG_TIER_1_BITS) - 1)
#define AQLOG_TIER_2_MASK               ((1 << AQLOG_TIER_2_BITS) - 1)

// The bit number number of bits per tier.
#define AQLOG_TIER_2_BITNUM             0
#define AQLOG_TIER_1_BITNUM             (AQLOG_TIER_2_BITNUM + AQLOG_TIER_2_BITS)
#define AQLOG_TIER_0_BITNUM             (AQLOG_TIER_1_BITNUM + AQLOG_TIER_1_BITS)

// The number of uint32_t words required for the hash memory.
// Each word contains 8 4-bit entries.
#define AQLOG_HASH_LEVEL_BITS_MUL_SHIFT 2
#define AQLOG_HASH_LEVEL_BITS           (1 << AQLOG_HASH_LEVEL_BITS_MUL_SHIFT)
#define AQLOG_HASH_LEVEL_MASK           ((1 << AQLOG_HASH_LEVEL_BITS) - 1)
#define AQLOG_HASH_WORD_BITS            32
#define AQLOG_HASH_INDEX_WORD_BITNUM    3
#define AQLOG_HASH_INDEX_LEVEL_MASK     ((1 << AQLOG_HASH_INDEX_WORD_BITNUM) - 1)
#define AQLOG_HASH_TABLE_WORDS          (1 << (  AQLOG_TIER_0_BITS              \
                                               + AQLOG_TIER_1_BITS              \
                                               + AQLOG_TIER_2_BITS              \
                                               - AQLOG_HASH_INDEX_WORD_BITNUM))

// The minimum acceptable size for the logging shared memory region.
#define AQLOG_SHM_MINIMUM_SIZE          (  10 * 1024 + AQLOG_HASH_TABLE_WORDS   \
                                         * sizeof(uint32_t))


// When allocating a record in the queue, allow for at least this number
// of bytes for the message text.  If the message is larger then the item will
// be extended.
#define AQLOG_RESERVE_MESSAGE_SIZE      100

// When allocating a record in the queue, if allocation fails try to reduce the 
// number of data bytes to this number.
#define AQLOG_DATA_TRUNCATE_SIZE        64

// The number of characters considered in the hash.  Characters are counted from
// the end of the string.
#define AQLOG_HASH_CHARMAX              32

// Used by the inline hash calculator to find the character to use at case
// position __index_ when the string is of length __size_.
//
// This adjusts the index lookup to account for strings that are longer
// than AQLOG_HASH_CHARMAX.
#define AQLOG_HASH_CHAR(__index_, __str_, __size_)                              \
    ((__str_)[  (__index_) - 1                                                  \
              + (((__size_) > AQLOG_HASH_CHARMAX)                               \
                    ? ((__size_) - AQLOG_HASH_CHARMAX) : 0)])


// Boolean - returns true if the passed character indicates the end of a hash
// string.
#define AQLOG_HASH_ISEND(__ch_)         ((__ch_) == '/' || (__ch_) == '\\')

// Maps an input character to a character actually used in the hash calculation.
// The characters used are never upper-case.
#define AQLOG_HASH_CHARMAP(__ch_)       (((__ch_) >= 'A' && (__ch_) <= 'Z') ? ((__ch_) - 'A' + 'a') : (__ch_))

// Implements a single case label of the inline hash function.
#define AQLOG_HASHTIER_INLINE_CASE(__index_, __hash_, __str_, __size_)          \
case __index_:                                                                  \
    if (AQLOG_HASH_ISEND(AQLOG_HASH_CHAR(__index_, __str_, __size_)))           \
    {                                                                           \
        break;                                                                  \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        __hash_ = AQLOG_HASH_STEP(__hash_,                                      \
                      AQLOG_HASH_CHARMAP(                                       \
                          AQLOG_HASH_CHAR(__index_, __str_, __size_)));         \
    }

// Calculates the hash value of a string __str_ consisting of __size_ characters
// in the string.  The hash is stored in the variable __hash_.
#define AQLOG_HASHTIER_INLINE(__hash_, __mask_, __str_, __size_, __rehashzero_) \
{                                                                               \
    if ((__size_) == 0 && (__rehashzero_))                                      \
    {                                                                           \
        __hash_ = 0;                                                            \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        __hash_ = AQLOG_HASH_INIT;                                              \
  	    switch (__size_)                                                        \
        {                                                                       \
      	    default:                                                            \
		    AQLOG_HASHTIER_INLINE_CASE(32, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(31, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(30, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(29, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(28, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(27, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(26, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(25, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(24, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(23, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(22, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(21, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(20, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(19, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(18, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(17, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(16, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(15, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(14, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(13, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(12, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(11, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE(10, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 9, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 8, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 7, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 6, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 5, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 4, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 3, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 2, __hash_, __str_, __size_)            \
		    AQLOG_HASHTIER_INLINE_CASE( 1, __hash_, __str_, __size_)            \
            case 0: break;                                                      \
        }                                                                       \
        __hash_ &= __mask_;                                                     \
        if (__hash_ == 0 && (__rehashzero_))                                    \
        {                                                                       \
            __hash_ = (__str_)[(__size_) - 1] & (__mask_);                      \
            if (__hash_ == 0)                                                   \
            {                                                                   \
                __hash_ = 1;                                                    \
            }                                                                   \
        }                                                                       \
    }                                                                           \
}

// Calculates the log level hash value for lookup strings __str1_, __str2_, and
// __str3_ using the inline calculation method.
#define AQLOG_HASHISLEVEL_INLINE(__level_, __str1_, __str2_, __str3_)           \
    AQLog_HashIsLevelInline(__level_, __str1_, sizeof(__str1_) - 1,             \
                                      __str2_, sizeof(__str2_) - 1,             \
                                      __str3_, sizeof(__str3_) - 1)


// Calculates the log level hash value for lookup strings __str1_, __str2_, and
// __str3_ using the external calculation method.
#define AQLOG_HASHISLEVEL_EXTERN(__level_, __str1_, __str2_, __str3_)           \
    AQLog_HashIsLevelExtern(__level_, __str1_, sizeof(__str1_) - 1,             \
                                      __str2_, sizeof(__str2_) - 1,             \
                                      __str3_, sizeof(__str3_) - 1)


// Helper macros for generating calls to __AQLog_Write() with log level pre-check.
#define AQLOG_WRITE(level, tagId, fmt, ...)                                     \
    AQLOG_WRITEDATA(level, tagId, NULL, 0, fmt, __VA_ARGS__)
#define AQLOG_WRITEDATA(level, tagId, data, dataSize, fmt, ...)                 \
do                                                                              \
{                                                                               \
    if (AQLOG_HASHISLEVEL(level, AQLOG_COMPONENT_ID, tagId, __FILE__))          \
    {                                                                           \
        __AQLog_Write(level, AQLOG_COMPONENT_ID, sizeof(AQLOG_COMPONENT_ID),    \
                      tagId, sizeof(tagId), __FILE__, sizeof(__FILE__),         \
                      __FUNCTION__, sizeof(__FUNCTION__), __LINE__,             \
                      data, dataSize, fmt, __VA_ARGS__);                        \
    }                                                                           \
} while (0)


/**
 * Writes a message at the AQLOG_LEVEL_CRITICAL level to the log.  The log
 * message is constructed from a printf-style formatting string and has no 
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Critical(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_CRITICAL, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_CRITICAL level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TCritical(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_CRITICAL, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_CRITICAL level to the log with an 
 * associated data dump.  The log message is constructed from a printf-style 
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DCritical(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_CRITICAL, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_CRITICAL level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDCritical(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_CRITICAL, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_ERROR level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Error(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_ERROR, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_ERROR level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TError(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_ERROR, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_ERROR level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DError(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_ERROR, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_ERROR level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDError(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_ERROR, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_WARNING level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Warning(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_WARNING, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_WARNING level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TWarning(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_WARNING, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_WARNING level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DWarning(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_WARNING, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_WARNING level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDWarning(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_WARNING, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_NOTICE level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Notice(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_NOTICE, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_NOTICE level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TNotice(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_NOTICE, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_NOTICE level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DNotice(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_NOTICE, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_NOTICE level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDNotice(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_NOTICE, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_INFO level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Info(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_INFO, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_INFO level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TInfo(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_INFO, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_INFO level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DInfo(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_INFO, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_INFO level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDInfo(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_INFO, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DETAIL level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Detail(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_DETAIL, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DETAIL level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDetail(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_DETAIL, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DETAIL level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DDetail(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_DETAIL, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DETAIL level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDDetail(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_DETAIL, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DEBUG level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Debug(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_DEBUG, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DEBUG level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDebug(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_DEBUG, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DEBUG level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DDebug(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_DEBUG, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_DEBUG level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDDebug(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_DEBUG, #tag, data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_TRACE level to the log.  The log
 * message is constructed from a printf-style formatting string and has no
 * associated tag or data dump.
 *
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_Trace(fmt, ...)                                                \
    AQLOG_WRITE(AQLOG_LEVEL_TRACE, "", fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_TRACE level to the log.  The log
 * message is constructed from a printf-style formatting string and is tagged
 * for the purpose of log filtering.  The log has no associated data dump.
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TTrace(tag, fmt, ...)                                        \
    AQLOG_WRITE(AQLOG_LEVEL_TRACE, #tag, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_TRACE level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string and has no associated tag.
 *
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_DTrace(data, dataSize, fmt, ...)                               \
    AQLOG_WRITEDATA(AQLOG_LEVEL_TRACE, "", data, dataSize, fmt, __VA_ARGS__)

/**
 * Writes a message at the AQLOG_LEVEL_TRACE level to the log with an
 * associated data dump.  The log message is constructed from a printf-style
 * formatting string  and is tagged for the purpose of log filtering
 *
 * @param tag (literal) The tag associated with this log record.  The tag is not
 * a string, is is just plain text that is converted into a string by this macro.
 * @param data (void *) A pointer to the data to process
 * @param dataSize (size_t) The number of data bytes to capture from the data
 * pointer to store in this log record.
 * @param fmt (const char *) The printf-style formatting string for the log message.
 * @param ... The formatting arguments for the log message.
 */
#define AQLog_TDTrace(tag, data, dataSize, fmt, ...)                         \
    AQLOG_WRITEDATA(AQLOG_LEVEL_TRACE, #tag, data, dataSize, fmt, __VA_ARGS__)




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

/**
 * Defines the logging levels.
 */
typedef enum AQLOG_LEVEL_T
{
    /** 
     * Critical situation - whole application failure is very likely.
     */
    AQLOG_LEVEL_CRITICAL    = 0,

    /**
     * An error condition occurred - it is likely to be recoverable.
     */
    AQLOG_LEVEL_ERROR       = 1,

    /**
     * Indicate that a serious condition occurred, but that the system has 
     * recovered.
     */
    AQLOG_LEVEL_WARNING     = 2,

    /**
     * A general notice of operation.
     */
    AQLOG_LEVEL_NOTICE      = 3,

    /**
     * Informational message.
     */
    AQLOG_LEVEL_INFO        = 4,

    /**
     * Detailed informational message.
     */
    AQLOG_LEVEL_DETAIL      = 5,

    /**
     * Developer debugging message.
     */
    AQLOG_LEVEL_DEBUG       = 6,

    /**
     * Low-level tracing message.
     */
    AQLOG_LEVEL_TRACE       = 7,

} AQLogLevel_t;


/**
* Defines the possible outcomes from initialising the AQLog.
*/
typedef enum AQLOG_INITOUTCOME_T
{
    /**
     * Log successfully initialised.
     */
    AQLOG_INITOUTCOME_SUCCESS = 0,

    /**
     * It was not possible to initialise the log because the length of the
     * shared memory region was insufficient to contain the required data
     * structures.
     */
    AQLOG_INITOUTCOME_SHM_TOO_SMALL = 1,

    /**
     * It was not possible to initialise the log because the AQ memory 
     * region had not been formatted to contain a valid AQ.
     */
    AQLOG_INITOUTCOME_AQ_UNFORMATTED = 2,

} AQLogInitOutcome_t;


//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------

// The hash table that is used to lookup the log level based on component, tag
// and line.
extern const uint32_t *AQLog_LevelHashTable_g;




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Calculates the log level hash value for lookup strings str1, str2, and
// str3 using the inline calculation method.  The lengths represent the number
// of characters in the string that participate in the hash.
AQLOG_HASH_INLINE_ATTRIBUTE bool AQLog_HashIsLevelInline(int level,
    const char *str1, size_t str1Size,
    const char *str2, size_t str2Size,
    const char *str3, size_t str3Size)
{
    uint32_t tier0Hash;
    AQLOG_HASHTIER_INLINE(tier0Hash, AQLOG_TIER_0_MASK, str1, str1Size, AQLOG_LOOKUP_TIER_TAGID == 0);

    uint32_t tier1Hash;
    AQLOG_HASHTIER_INLINE(tier1Hash, AQLOG_TIER_1_MASK, str2, str2Size, AQLOG_LOOKUP_TIER_TAGID == 1);

    uint32_t tier2Hash;
    AQLOG_HASHTIER_INLINE(tier2Hash, AQLOG_TIER_2_MASK, str3, str3Size, AQLOG_LOOKUP_TIER_TAGID == 2);

    uint32_t index = (tier0Hash << AQLOG_TIER_0_BITNUM)
        | (tier1Hash << AQLOG_TIER_1_BITNUM)
        | (tier2Hash << AQLOG_TIER_2_BITNUM);
    uint32_t word = index >> AQLOG_HASH_INDEX_WORD_BITNUM;
    uint32_t bitnum = (index & AQLOG_HASH_INDEX_LEVEL_MASK) << AQLOG_HASH_LEVEL_BITS_MUL_SHIFT;

    return (AQLog_LevelHashTable_g[word] & (AQLOG_HASH_LEVEL_MASK << bitnum)) >= ((uint32_t)level << bitnum);
}

// Calculates the log level hash value for lookup strings str1, str2, and
// str3 using the exernal function calculation method.  The lengths represent 
// the number of characters in the string that participate in the hash.
extern "C" AQLOG_HASH_EXTERN_ATTRIBUTE bool AQLog_HashIsLevelExtern(int level, const char *str1, size_t str1Size, const char *str2, size_t str2Size, const char *str3, size_t str3Size);

// Used to initialise the log producer directly from a shared memory region.
// This should not be called by applications.
class IAQSharedMemory;
extern "C" AQLogInitOutcome_t AQLog_InitSharedMemory(IAQSharedMemory& sm);

/**
 * Deinitialise and clean-up the AQLog interface.  Once this function is called
 * the logging interface becomes inoperative.  Any attempt to access the logging
 * interface after this point results in undefined behavior (likey a crash).
 */
extern "C" void AQLog_Deinit(void);

// The single entry-point for actually writing a log message into the shared
// log buffer.  
extern "C" void __AQLog_Write(AQLogLevel_t level, const char *componentId,
    size_t componentIdSize, const char *tagId, size_t tagIdSize,
    const char *file, size_t fileSize, const char *func, size_t funcSize,
    int line, const void *data, size_t dataSize, const char *msg, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 13, 14)))
#endif
    ;


/*
#include <stdio.h>
void testInline(void)
{
    if (AQLOG_HASHISLEVEL_INLINE(7, "fozzie", "fooble", __FILE__))
    {
        printf("hello world");
    }
}
*/




#endif
//=============================== End of File ==================================
