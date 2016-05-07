#ifndef AQLOG_H
#define AQLOG_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

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
#define AQLOG_HASH_EXTERN_ATTRIBUTE     __attribute__((pure))

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



//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

typedef enum AQLOG_LEVEL_T
{
    AQLOG_LEVEL_CRITICAL    = 0,
    AQLOG_LEVEL_ERROR       = 1,
    AQLOG_LEVEL_WARNING     = 2,
    AQLOG_LEVEL_NOTICE      = 3,
    AQLOG_LEVEL_INFO        = 4,
    AQLOG_LEVEL_DETAIL      = 5,
    AQLOG_LEVEL_DEBUG       = 6,
    AQLOG_LEVEL_TRACE       = 7,
} AQLogLevel_t;




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
extern AQLOG_HASH_EXTERN_ATTRIBUTE bool AQLog_HashIsLevelExtern(int level, const char *str1, size_t str1Size, const char *str2, size_t str2Size, const char *str3, size_t str3Size);

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
