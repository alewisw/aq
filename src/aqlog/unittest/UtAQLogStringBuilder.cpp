//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"

#include "AQLogStringBuilder.h"

#ifdef WIN32
#define strdup _strdup
#endif




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

#define LONG_STR                ("Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?")
#define LONG_STR_LEN            ((sizeof(LONG_STR) - 1))
static const char LONG_STR_BUF[] = LONG_STR;

#define FRAG_STR                ("At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga.")
#define FRAG_STR_LEN            ((sizeof(FRAG_STR) - 1))
static const char FRAG_STR_BUF[] = FRAG_STR;

#define INSERT_EMPTY_WRAPPER(pos, str)          memcpy(msg.insertEmpty(pos, sizeof(str) - 1), str, sizeof(str) - 1)
#define INSERT_EMPTY_WRAPPER_LEN(pos, str, len) memcpy(msg.insertEmpty(pos, len), str, len)

#define APPEND_EMPTY_WRAPPER(str)               memcpy(msg.appendEmpty(sizeof(str) - 1), str, sizeof(str) - 1)
#define APPEND_EMPTY_WRAPPER_LEN(str, len)      memcpy(msg.appendEmpty(len), str, len)

static const time_t STRFTIME_TEST_TIME = 238447192;
#define STRFTIME_TEST_TIME_FMT "%Y-%m-%d %H:%M:%S"
#define STRFTIME_TEST_TIME_STR "1977-07-23 03:19:52"




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Adds the entirity of the LONG_STR string into 'msg' as a series of many fragments.
// Returns the number of fragments added.
static size_t makeFragmentedStringBuilder(AQLogStringBuilder& msg);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtAQLogStringBuilder);

//------------------------------------------------------------------------------
static size_t makeFragmentedStringBuilder(AQLogStringBuilder& msg)
{
    size_t pos = 0;
    size_t fragCount = 0;
    while (pos < FRAG_STR_LEN)
    {
        size_t len = 1 + rand() % (FRAG_STR_LEN / 10);
        if (len > FRAG_STR_LEN - pos)
        {
            len = FRAG_STR_LEN - pos;
        }
        if (rand() % 2)
        {
            msg.insertCopy(msg.end(), &(FRAG_STR)[pos], len);
        }
        else
        {
            msg.insertPointer(msg.end(), &(FRAG_STR)[pos], len);
        }
        pos += len;
        fragCount++;
    }
    REQUIRE(msg.size() == FRAG_STR_LEN);
    return fragCount;
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_CopyConstructed_then_NewSbEmpty)
{
    AQLogStringBuilder msg(0);

    AQLogStringBuilder cpy(msg);

    REQUIRE(0 == cpy.size());
    REQUIRE(cpy.toString() == "");
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_CopyConstructed_then_NewSbIdentical)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder cpy(msg);

    REQUIRE(msg.size() == cpy.size());
    REQUIRE(msg.toString() == cpy.toString());
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_Assigned_then_NewSbIdentical)
{
    AQLogStringBuilder msg(0);

    AQLogStringBuilder cpy;
    makeFragmentedStringBuilder(cpy);
    cpy = msg;

    REQUIRE(0 == cpy.size());
    REQUIRE(cpy.toString() == "");
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_Assigned_then_NewSbIdentical)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder cpy;
    cpy.appendPointer("ullamco laboris nisi ut aliquip ex ea commodo consequat");
    cpy = msg;

    REQUIRE(msg.size() == cpy.size());
    REQUIRE(msg.toString() == cpy.toString());
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_Clear_SbEmpty)
{
    AQLogStringBuilder msg(0);
    msg.clear();
    REQUIRE(msg.toString() == "");
    REQUIRE(msg.size() == 0);
    REQUIRE(msg.iovCount() == 0);
}

//------------------------------------------------------------------------------
TEST(given_SbCopySingle_when_Clear_SbEmpty)
{
    AQLogStringBuilder msg(0);
    msg.insertCopy(msg.begin(), "in voluptate velit esse cillum");
    msg.clear();
    REQUIRE(msg.toString() == "");
    REQUIRE(msg.size() == 0);
    REQUIRE(msg.iovCount() == 0);
}

//------------------------------------------------------------------------------
TEST(given_SbCopyLongSingle_when_Clear_SbEmpty)
{
    AQLogStringBuilder msg(0);
    msg.insertCopy(msg.begin(), LONG_STR);
    msg.clear();
    REQUIRE(msg.toString() == "");
    REQUIRE(msg.size() == 0);
    REQUIRE(msg.iovCount() == 0);
}

//------------------------------------------------------------------------------
TEST(given_SbCopyFragmented_when_Clear_SbEmpty)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);
    msg.clear();
    REQUIRE(msg.toString() == "");
    REQUIRE(msg.size() == 0);
    REQUIRE(msg.iovCount() == 0);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertPointer_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem ipsum dolor sit amet"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertPointerAtEnd_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem ipsum"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 11, " dolor sit amet"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertPointerAtStart_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "dolor sit amet"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem ipsum "));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertPointerInMiddle_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem  sit amet"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, "ipsum dolor"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertPointerAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " sit amet"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, "ipsum dolor"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertZeroLengthPointerAtSplit_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "ipsum dolor sit amet"));
    REQUIRE(msg.iovCount() == 2);
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ""));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertNullPointerAtSplit_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Lorem "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "ipsum dolor sit amet"));
    REQUIRE(msg.iovCount() == 2);
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, NULL));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, NULL, 1000));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_InsertPointerAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), LONG_STR, LONG_STR_LEN / 3));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), &LONG_STR[2 * LONG_STR_LEN / 3]));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + LONG_STR_LEN / 3, &LONG_STR[LONG_STR_LEN / 3], (2 * LONG_STR_LEN / 3) - (LONG_STR_LEN / 3)));
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_SmallInsertPointerAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), LONG_STR, LONG_STR_LEN / 2));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), &LONG_STR[LONG_STR_LEN / 2 + 10]));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + LONG_STR_LEN / 2, &LONG_STR[LONG_STR_LEN / 2], 10));
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), "consectetur adipiscing elit"));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertHugeCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    ostringstream ss;
    size_t total = 0;
    while (total < 256 * 1024)
    {
        REQUIRE(&msg == &msg.insertCopy(msg.end(), LONG_STR));
        ss << LONG_STR;
        total += LONG_STR_LEN;
    }
    REQUIRE(msg.toString() == ss.str());
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertCopyAtEnd_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "consectetur adipiscing"));
    REQUIRE(&msg == &msg.insertCopy(msg.end(), " elit"));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertCopyAtStart_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), " adipiscing elit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), string("consectetur")));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertCopyInMiddle_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "consectetur elit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 12, "adipiscing "));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertCopyAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "consectetur"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " elit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 11, " adipiscing"));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertZeroLengthCopyAtSplit_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), "consectetur "));
    REQUIRE(&msg == &msg.insertCopy(msg.end(), "adipiscing elit"));
    REQUIRE(msg.iovCount() == 2);
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ""));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, "foo", 0));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertNullCopyAtSplit_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), "consectetur "));
    REQUIRE(&msg == &msg.insertCopy(msg.end(), "adipiscing elit"));
    REQUIRE(msg.iovCount() == 2);
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, NULL));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, NULL, 1000));
    REQUIRE(msg.toString() == "consectetur adipiscing elit");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_InsertCopyAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), LONG_STR, LONG_STR_LEN / 4));
    REQUIRE(&msg == &msg.insertCopy(msg.end(), &LONG_STR[3 * LONG_STR_LEN / 4]));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + LONG_STR_LEN / 4, &LONG_STR[LONG_STR_LEN / 4], (3 * LONG_STR_LEN / 4) - (LONG_STR_LEN / 4)));
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_SmallInsertCopyAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), LONG_STR, LONG_STR_LEN / 3));
    REQUIRE(&msg == &msg.insertCopy(msg.end(), &LONG_STR[LONG_STR_LEN / 3 + 10]));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + LONG_STR_LEN / 3, &LONG_STR[LONG_STR_LEN / 3], 10));
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertFree_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertFree(msg.begin(), strdup("sed do eiusmod tempor incididunt")));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertFreeAtEnd_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "s"));
    REQUIRE(&msg == &msg.insertFree(msg.end(), strdup("ed do eiusmod tempor incididunt")));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertFreeAtStart_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "t"));
    REQUIRE(&msg == &msg.insertFree(msg.begin(), strdup("sed do eiusmod tempor incididun")));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertFreeInMiddle_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "st"));
    REQUIRE(&msg == &msg.insertFree(msg.begin() + 1, strdup("ed do eiusmod tempor incididun")));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertFreeAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "s"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "t"));
    REQUIRE(&msg == &msg.insertFree(msg.begin() + 1, strdup("ed do eiusmod tempor incididun")));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertZeroLengthFree_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertFree(msg.begin(), strdup("s")));
    REQUIRE(&msg == &msg.insertFree(msg.end(), strdup("ed do eiusmod tempor incididunt")));
    REQUIRE(msg.iovCount() == 2);
    char *strempty = strdup("");
    char *strfoo = strdup("foo");
    REQUIRE(&msg == &msg.insertFree(msg.begin() + 10, strempty));
    REQUIRE(&msg == &msg.insertFree(msg.begin() + 10, strfoo, 0));
    free(strempty);
    free(strfoo);
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertNullFreeAt_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertFree(msg.begin(), strdup("sed do eiusmod tempor incididun")));
    REQUIRE(&msg == &msg.insertFree(msg.end(), strdup("t")));
    REQUIRE(msg.iovCount() == 2);
    REQUIRE(&msg == &msg.insertFree(msg.begin() + 6, NULL));
    REQUIRE(&msg == &msg.insertFree(msg.begin() + 6, NULL, 1000));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_InsertFreeAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), LONG_STR, LONG_STR_LEN / 5));
    REQUIRE(&msg == &msg.insertFree(msg.end(), strdup(&LONG_STR[4 * LONG_STR_LEN / 5])));
    char *s = (char *)malloc((4 * LONG_STR_LEN / 5) - (LONG_STR_LEN / 5) + 1);
    memcpy(s, &LONG_STR[LONG_STR_LEN / 5], (4 * LONG_STR_LEN / 5) - (LONG_STR_LEN / 5));
    s[(4 * LONG_STR_LEN / 5) - (LONG_STR_LEN / 5)] = '\0';
    REQUIRE(&msg == &msg.insertFree(msg.begin() + LONG_STR_LEN / 5, s));
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_SmallInsertFreeAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), LONG_STR, LONG_STR_LEN / 4));
    REQUIRE(&msg == &msg.insertFree(msg.end(), strdup(&LONG_STR[LONG_STR_LEN / 4 + 10])));
    char *s = (char *)malloc(11);
    memcpy(s, &LONG_STR[LONG_STR_LEN / 4], 10);
    s[10] = '\0';
    REQUIRE(&msg == &msg.insertFree(msg.begin() + LONG_STR_LEN / 4, s));
    REQUIRE(msg.toString() == LONG_STR);
}


//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertEmpty_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    INSERT_EMPTY_WRAPPER(msg.begin(), "ut labore et dolore magna aliqua");
    REQUIRE(msg.toString() == "ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptyAtEnd_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ut labore et dolore magna aliqu"));
    INSERT_EMPTY_WRAPPER(msg.end(), "a");
    REQUIRE(msg.toString() == "ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptyAtStart_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "t labore et dolore magna aliqua"));
    INSERT_EMPTY_WRAPPER(msg.begin(), "u");
    REQUIRE(msg.toString() == "ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptyInMiddle_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ut labore et doore magna aliqua"));
    INSERT_EMPTY_WRAPPER(msg.begin() + 15, "l");
    REQUIRE(msg.toString() == "ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertEmptyAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ut labore et dol"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "re magna aliqua"));
    INSERT_EMPTY_WRAPPER(msg.begin() + 16, "o");
    REQUIRE(msg.toString() == "ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertZeroLengthEmpty_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ut labore et dol"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "ore magna aliqua"));

    REQUIRE(msg.iovCount() == 2);
    REQUIRE((msg.insertEmpty(msg.begin() + 4, 0) == NULL));
    REQUIRE(msg.toString() == "ut labore et dolore magna aliqua");
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_InsertEmptyAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    INSERT_EMPTY_WRAPPER_LEN(msg.begin(), LONG_STR, LONG_STR_LEN / 6);
    INSERT_EMPTY_WRAPPER_LEN(msg.end(), &LONG_STR[5 * LONG_STR_LEN / 6], LONG_STR_LEN - 5 * LONG_STR_LEN / 6);
    INSERT_EMPTY_WRAPPER_LEN(msg.begin() + LONG_STR_LEN / 6, &LONG_STR[LONG_STR_LEN / 6], (5 * LONG_STR_LEN / 6) - (LONG_STR_LEN / 6));
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbLongSplit_when_SmallInsertEmptyAtSplit_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    INSERT_EMPTY_WRAPPER_LEN(msg.begin(), LONG_STR, LONG_STR_LEN / 4);
    INSERT_EMPTY_WRAPPER_LEN(msg.end(), &LONG_STR[LONG_STR_LEN / 4 + 10], LONG_STR_LEN - LONG_STR_LEN / 4 - 10);
    INSERT_EMPTY_WRAPPER_LEN(msg.begin() + LONG_STR_LEN / 4, &LONG_STR[LONG_STR_LEN / 4], 10);
    REQUIRE(msg.toString() == LONG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_EraseBeginNonZero_ContentEmpty)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.erase(msg.begin(), 12));
    REQUIRE(msg.size() == 0);
    REQUIRE(msg.toString() == "");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_EraseEndNonZero_ContentEmpty)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.erase(msg.end(), 7));
    REQUIRE(msg.size() == 0);
    REQUIRE(msg.toString() == "");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_EraseBegin_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut enim ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.begin(), 7));
    REQUIRE(msg.toString() == " ad minim veniam");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_EraseEnd_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut enim ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.end() - 4, 4));
    REQUIRE(msg.toString() == "Ut enim ad minim ve");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_EraseBeyondEnd_ErasedToEnd)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut enim ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.end() - 7, 100));
    REQUIRE(msg.toString() == "Ut enim ad minim");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_EraseMiddle_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut enim ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.begin() + 10, 5));
    REQUIRE(msg.toString() == "Ut enim adm veniam");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_EraseAtSplit_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "enim ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.begin() + 3, 5));
    REQUIRE(msg.toString() == "Ut ad minim veniam");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_EraseAfterSplit_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut enim "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.begin() + 10, 3));
    REQUIRE(msg.toString() == "Ut enim adnim veniam");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_EraseAcrossSplit_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut eni"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "m ad minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.begin() + 3, 12));
    REQUIRE(msg.toString() == "Ut m veniam");
}

//------------------------------------------------------------------------------
TEST(given_SbDoubleSplit_when_EraseAcrossTwoSplits_RegionErased)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "Ut en"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), "im ad"));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " minim veniam"));
    REQUIRE(&msg == &msg.erase(msg.begin() + 1, 14));
    REQUIRE(msg.toString() == "Um veniam");
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PreIncrementAfterEnd_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.end() - 1;
    REQUIRE(it != msg.end());
    ++it;
    REQUIRE(it == msg.end());
    ++it;
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PreIncrementIteratorForwards_EachByteEncountered)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.begin();
    REQUIRE(it == msg.begin());
    for (size_t i = 0; i < FRAG_STR_LEN; ++i)
    {
        REQUIRE(*it == FRAG_STR[i]);
        ++it;
    }
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PostIncrementAfterEnd_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.end() - 1;
    REQUIRE(it != msg.end());
    it++;
    REQUIRE(it == msg.end());
    it++;
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PostIncrementIteratorForwards_EachByteEncountered)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.begin();
    REQUIRE(it == msg.begin());
    for (size_t i = 0; i < FRAG_STR_LEN; ++i)
    {
        REQUIRE(*it++ == FRAG_STR[i]);
    }
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PreDecrementBeforeBegin_IteratorAtBegin)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.begin() + 1;
    REQUIRE(it != msg.begin());
    --it;
    REQUIRE(it == msg.begin());
    --it;
    REQUIRE(it == msg.begin());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PreDecrementIteratorBackwards_EachByteEncountered)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.end();
    REQUIRE(it == msg.end());
    for (size_t i = FRAG_STR_LEN; i > 0; --i)
    {
        --it;
        REQUIRE(*it == FRAG_STR[i - 1]);
    }
    REQUIRE(it == msg.begin());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PostDecrementBeforeBegin_IteratorAtBegin)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.begin() + 1;
    REQUIRE(it != msg.begin());
    it--;
    REQUIRE(it == msg.begin());
    it--;
    REQUIRE(it == msg.begin());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_PostDecrementIteratorBackwards_EachByteEncountered)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it = msg.end() - 1;
    REQUIRE(it == msg.end() - 1);
    for (size_t i = FRAG_STR_LEN; i > 0; --i)
    {
        REQUIRE(*it-- == FRAG_STR[i - 1]);
    }
    REQUIRE(it == msg.begin());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_IncrementIteratorForwardsPastEnd_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    for (size_t n = 0; n <= FRAG_STR_LEN; ++n)
    {
        AQLogStringBuilder::iterator it = msg.end() - n;
        it += n + 1;
        REQUIRE(it == msg.end());
        it += n + 1;
        REQUIRE(it == msg.end());
    }
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_IncrementIteratorForwardsByN_BytePlusNEncountered)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    for (size_t n = 1; n < FRAG_STR_LEN; ++n)
    {
        for (size_t i = 0; i < FRAG_STR_LEN - n; ++i)
        {
            AQLogStringBuilder::iterator it = msg.begin() + i;
            it += n;
            REQUIRE(*it == FRAG_STR[i + n]);
        }
    }
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_DecrementIteratorForwardsPastBegin_IteratorAtBegin)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    for (size_t n = 0; n <= FRAG_STR_LEN; ++n)
    {
        AQLogStringBuilder::iterator it = msg.begin() + n;
        it -= n + 1;
        REQUIRE(it == msg.begin());
        it -= n + 1;
        REQUIRE(it == msg.begin());
    }
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_DecrementIteratorBackwardsByN_ByteSubtractNEncountered)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    for (size_t n = 1; n < FRAG_STR_LEN; ++n)
    {
        for (size_t i = FRAG_STR_LEN; i >= n; --i)
        {
            AQLogStringBuilder::iterator it = msg.begin() + i;
            it -= n;
            REQUIRE(*it == FRAG_STR[i - n]);
        }
    }
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_TwoIteratorsCompared_OrderingCorrect)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it1 = msg.begin();
    for (size_t i = 0; i < FRAG_STR_LEN; ++i)
    {
        AQLogStringBuilder::iterator it2 = msg.begin();
        for (size_t j = 0; j < FRAG_STR_LEN; ++j)
        {
            if (i == j)
            {
                REQUIRE((it1 == it2));
                REQUIRE(!(it1 != it2));
                REQUIRE(!(it1 > it2));
                REQUIRE((it1 >= it2));
                REQUIRE(!(it1 < it2));
                REQUIRE((it1 <= it2));
            }
            else if (i < j)
            {
                REQUIRE(!(it1 == it2));
                REQUIRE((it1 != it2));
                REQUIRE(!(it1 > it2));
                REQUIRE(!(it1 >= it2));
                REQUIRE((it1 < it2));
                REQUIRE((it1 <= it2));
            }
            else
            {
                REQUIRE(!(it1 == it2));
                REQUIRE((it1 != it2));
                REQUIRE((it1 > it2));
                REQUIRE((it1 >= it2));
                REQUIRE(!(it1 < it2));
                REQUIRE(!(it1 <= it2));
            }
            it2++;
        }
        REQUIRE(it2 == msg.end());
        it1++;
    }
    REQUIRE(it1 == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_TwoIteratorsSubtracted_DifferenceCorrect)
{
    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    AQLogStringBuilder::iterator it1 = msg.begin();
    for (size_t i = 0; i < FRAG_STR_LEN; ++i)
    {
        AQLogStringBuilder::iterator it2 = msg.begin();
        for (size_t j = 0; j < FRAG_STR_LEN; ++j)
        {
            REQUIRE(it1 - it2 == ((i >= j) ? (i - j) : (j - i)));
            REQUIRE(it2 - it1 == ((i >= j) ? (i - j) : (j - i)));

            it2++;
        }
        REQUIRE(it2 == msg.end());
        it1++;
    }
    REQUIRE(it1 == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_FindChar_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    REQUIRE(msg.end() == msg.find('x'));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindCharNotInBuffer_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    REQUIRE(msg.end() == msg.find('v'));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindCharFirstInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    AQLogStringBuilder::iterator it = msg.find('q');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'q');
    REQUIRE(*it == 'u');
    REQUIRE(msg.end() == msg.find(it, 'q'));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindCharLastInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    AQLogStringBuilder::iterator it = msg.find('n');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'n');
    REQUIRE(*it == 'o');
    it = msg.find(it, 'n');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'n');
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindCharInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    AQLogStringBuilder::iterator it = msg.find('u');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'u');
    REQUIRE(*it == 'i');
    it = msg.find(it, 'u');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'u');
    REQUIRE(*it == 'd');
    REQUIRE(msg.end() == msg.find(it, 'u'));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindCharInsideSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "qui");
    msg.insertPointer(msg.end(), "s nostrud e");
    msg.insertPointer(msg.end(), "xercitation");
    AQLogStringBuilder::iterator it = msg.find('u');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'u');
    REQUIRE(*it == 'i');
    it = msg.find(it, 'u');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'u');
    REQUIRE(*it == 'd');
    REQUIRE(msg.end() == msg.find(it, 'u'));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindCharBeforeSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "qui");
    msg.insertPointer(msg.end(), "s nostrud exerci");
    msg.insertPointer(msg.end(), "tati");
    msg.insertPointer(msg.end(), "on");
    AQLogStringBuilder::iterator it = msg.find('i');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'i');
    REQUIRE(*it == 's');
    it = msg.find(it, 'i');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'i');
    REQUIRE(*it == 't');
    it = msg.find(it, 'i');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'i');
    REQUIRE(*it == 'o');
    REQUIRE(msg.end() == msg.find(it, 'i'));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindCharAfterSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "qu");
    msg.insertPointer(msg.end(), "is nostrud exerc");
    msg.insertPointer(msg.end(), "itat");
    msg.insertPointer(msg.end(), "ion");
    AQLogStringBuilder::iterator it = msg.find('i');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'i');
    REQUIRE(*it == 's');
    it = msg.find(it, 'i');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'i');
    REQUIRE(*it == 't');
    it = msg.find(it, 'i');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'i');
    REQUIRE(*it == 'o');
    REQUIRE(msg.end() == msg.find(it, 'i'));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindSingleCharFirstInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "qui");
    msg.insertPointer(msg.end(), "is nostrud exerc");
    msg.insertPointer(msg.end(), "itat");
    msg.insertPointer(msg.end(), "ion");
    AQLogStringBuilder::iterator it = msg.find('q');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'q');
    REQUIRE(*it == 'u');
    REQUIRE(msg.end() == msg.find(it, 'q'));
}

//------------------------------------------------------------------------------
TEST(given_SbSplitFirstVec1Char_when_FindSingleCharFirstInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "q");
    msg.insertPointer(msg.end(), "uis nostrud exerc");
    msg.insertPointer(msg.end(), "itat");
    msg.insertPointer(msg.end(), "ion");
    AQLogStringBuilder::iterator it = msg.find('q');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'q');
    REQUIRE(*it == 'u');
    REQUIRE(msg.end() == msg.find(it, 'q'));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindCharLastInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "qu");
    msg.insertPointer(msg.end(), "is nostrud exerc");
    msg.insertPointer(msg.end(), "itat");
    msg.insertPointer(msg.end(), "ion");
    AQLogStringBuilder::iterator it = msg.find('n');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'n');
    REQUIRE(*it == 'o');
    it = msg.find(it, 'n');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'n');
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbSplitLastVec1Char_when_FindCharLastInBuffer_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "qu");
    msg.insertPointer(msg.end(), "is nostrud exerc");
    msg.insertPointer(msg.end(), "itatio");
    msg.insertPointer(msg.end(), "n");
    AQLogStringBuilder::iterator it = msg.find('n');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'n');
    REQUIRE(*it == 'o');
    it = msg.find(it, 'n');
    REQUIRE(it != msg.end());
    REQUIRE(*it++ == 'n');
    REQUIRE(it == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_FindStr_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    REQUIRE(msg.end() == msg.find("x"));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindStrNotInBuffer_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "ullamco laboris nisi ut aliquip");
    REQUIRE(msg.end() == msg.find("ipsum"));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindStrNull_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "ullamco laboris nisi ut aliquip");
    REQUIRE(msg.end() == msg.find((char *)NULL));
    REQUIRE(msg.end() == msg.find(msg.begin() + 1, (char *)NULL));
    REQUIRE(msg.end() == msg.find(msg.end() - 1, (char *)NULL));
    REQUIRE(msg.end() == msg.find(msg.end(), (char *)NULL));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindStrEmpty_IteratorAtEnd)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "ullamco laboris nisi ut aliquip");
    AQLogStringBuilder::iterator it = msg.begin();
    REQUIRE(msg.end() == msg.find(""));
    REQUIRE(msg.end() == msg.find(msg.begin() + 1, ""));
    REQUIRE(msg.end() == msg.find(msg.end() - 1, ""));
    REQUIRE(msg.end() == msg.find(msg.end(), ""));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindStrAtStart_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "ullamco laboris nisi ut aliquip");
    AQLogStringBuilder::iterator it = msg.find("ullamco");
    REQUIRE(it == msg.begin());
    REQUIRE(msg.end() == msg.find(it + 1, "ullamco"));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindStrInMiddle_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "ullamco laboris nisi ut aliquip");
    AQLogStringBuilder::iterator it = msg.find("nisi");
    REQUIRE(it == msg.begin() + 16);
    REQUIRE((it = msg.find(it, "ut")) == msg.begin() + 21);
    REQUIRE(msg.end() == msg.find(it, "nisi"));
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_FindStrAtEnd_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "ullamco laboris nisi ut aliquip");
    AQLogStringBuilder::iterator it = msg.find("aliquip");
    REQUIRE(it == msg.end() - 7);
    REQUIRE(msg.end() == msg.find(it + 1, "aliquip"));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindStrInMiddle_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "ullamc");
    msg.insertPointer(msg.end(), "o laboris nisi u");
    msg.insertPointer(msg.end(), "t aliquip");
    AQLogStringBuilder::iterator it = msg.find("laboris");
    REQUIRE(it == msg.begin() + 8);
    REQUIRE((it = msg.find(it + 1, "nisi")) == msg.begin() + 16);
    REQUIRE(msg.end() == msg.find(it, "laboris"));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindStrBeforeSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "ullamco laboris");
    msg.insertPointer(msg.end(), " nisi ut");
    msg.insertPointer(msg.end(), " aliquip");
    AQLogStringBuilder::iterator it = msg.find(string("laboris"));
    REQUIRE(it == msg.begin() + 8);
    REQUIRE((it = msg.find(it + 1, string("ut"))) == msg.begin() + 21);
    REQUIRE(msg.end() == msg.find(it, string("laboris")));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindStrAfterSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "ullamco ");
    msg.insertPointer(msg.end(), "laboris nisi ");
    msg.insertPointer(msg.end(), "ut aliquip");
    AQLogStringBuilder::iterator it = msg.find("laboris");
    REQUIRE(it == msg.begin() + 8);
    REQUIRE((it = msg.find(it + 1, "ut")) == msg.begin() + 21);
    REQUIRE(msg.end() == msg.find(it, "laboris"));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindStrAcrossSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "ullamco lab");
    msg.insertPointer(msg.end(), "oris nisi u");
    msg.insertPointer(msg.end(), "t aliquip");
    AQLogStringBuilder::iterator it = msg.find("laboris");
    REQUIRE(it == msg.begin() + 8);
    REQUIRE((it = msg.find(it + 1, "ut")) == msg.begin() + 21);
    REQUIRE(msg.end() == msg.find(it, "laboris"));
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_FindStrAcrossMultipleSplit_IteratorAtChar)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.end(), "u");
    msg.insertPointer(msg.end(), "ll");
    msg.insertPointer(msg.end(), "amco l");
    msg.insertPointer(msg.end(), "abori");
    msg.insertPointer(msg.end(), "s nisi ut ali");
    msg.insertPointer(msg.end(), "q");
    msg.insertPointer(msg.end(), "uip");
    AQLogStringBuilder::iterator it = msg.find("ullamco");
    REQUIRE(it == msg.begin());
    REQUIRE((it = msg.find(it, "laboris")) == msg.begin() + 8);
    REQUIRE((it = msg.find(it, "aliquip")) == msg.begin() + 24);
    REQUIRE((it = msg.find(it, "ut")) == msg.end());
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_Find40CharStrings_StringsFound)
{
    const size_t len = 40;
    char str[len + 1];

    AQLogStringBuilder msg(0);
    makeFragmentedStringBuilder(msg);

    str[len] = '\0';
    for (size_t i = 0; i < FRAG_STR_LEN - len; ++i)
    {
        memcpy(str, &(FRAG_STR)[i], len);
        REQUIRE(msg.find(str) == msg.begin() + i);
    }
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptySbPointer_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 7, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptySbPointerSubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAABBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 7, ins.begin() + 3, ins.begin() + 3));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertBeginEndReversedSbPointerSubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAABBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 7, ins.begin() + 4, ins.begin() + 1));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSingleSbPointer_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSplitSbPointer_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "ex ea commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor in reprehenderit");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertDoubleSplitSbPointer_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "ex ea commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure ");
    ins.insertPointer(ins.end(), "dolor in reprehenderit");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSingleSbPointerSubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAAex ea commodo consequat. Duis aute irure dolor in reprehenderitBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSplitSbPointerSubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAAex ea commodo consequat. Duis aute irure ");
    ins.insertPointer(ins.end(), "dolor in reprehenderitBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertDoubleSplitSbPointerSubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAAex ea commodo ");
    ins.insertPointer(ins.end(), "consequat. Duis aute irure ");
    ins.insertPointer(ins.end(), "dolor in reprehenderitBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSingleSbPointerAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSplitSbPointerAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbPointerAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSingleSbPointerSubstrAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. Duis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSplitSbPointerSubstrAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor inD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin() + 4, ins.end() - 1));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbPointerSubstrAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "Ccommodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin() + 1, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbPointerSubstrAtStart_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "JJJJJJex ea commodo ");
    ins.insertPointer(ins.end(), "consequat. Duis aute ");
    ins.insertPointer(ins.end(), "irure dolorKKKKKKKKKKKK");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), " in reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), ins.begin() + 6, ins.end() - 12));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbPointerSubstrAtEnd_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "NNNcommodo consequ");
    ins.insertPointer(ins.end(), "at. Duis aute irure dolor in reprehenderi");
    ins.insertPointer(ins.end(), "tMM");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), ins.begin() + 3, ins.end() - 2));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSingleSbPointerAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSplitSbPointerAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertDoubleSplitSbPointerAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSingleSbPointerSubstrAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. Duis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSplitSbPointerSubstrAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor inD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin() + 4, ins.end() - 1));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertDoubleSplitSbPointerSubstrAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "Ccommodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertPointer(msg.begin() + 6, ins.begin() + 1, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptySbCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 7, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertEmptySbCopySubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAABBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 7, ins.begin() + 3, ins.begin() + 3));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertBeginEndReversedSbCopySubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAABBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 7, ins.begin() + 4, ins.begin() + 1));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSingleSbCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSplitSbCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "ex ea commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor in reprehenderit");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertDoubleSplitSbCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "ex ea commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure ");
    ins.insertPointer(ins.end(), "dolor in reprehenderit");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSingleSbCopySubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAAex ea commodo consequat. Duis aute irure dolor in reprehenderitBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertSplitSbCopySubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAAex ea commodo consequat. Duis aute irure ");
    ins.insertPointer(ins.end(), "dolor in reprehenderitBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_InsertDoubleSplitSbCopySubstr_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAAex ea commodo ");
    ins.insertPointer(ins.end(), "consequat. Duis aute irure ");
    ins.insertPointer(ins.end(), "dolor in reprehenderitBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSingleSbCopyAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSplitSbCopyAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbCopyAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSingleSbCopySubstrAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. Duis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertSplitSbCopySubstrAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor inD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin() + 4, ins.end() - 1));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbCopySubstrAtMid_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "Ccommodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea  reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin() + 1, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbCopySubstrAtStart_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "JJJJJJex ea commodo ");
    ins.insertPointer(ins.end(), "consequat. Duis aute ");
    ins.insertPointer(ins.end(), "irure dolorKKKKKKKKKKKK");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), " in reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin(), ins.begin() + 6, ins.end() - 12));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertDoubleSplitSbCopySubstrAtEnd_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "NNNcommodo consequ");
    ins.insertPointer(ins.end(), "at. Duis aute irure dolor in reprehenderi");
    ins.insertPointer(ins.end(), "tMM");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertCopy(msg.end(), ins.begin() + 3, ins.end() - 2));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSingleSbCopyAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSplitSbCopyAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertDoubleSplitSbCopyAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "commodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor in");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSingleSbCopySubstrAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. Duis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin() + 4, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertSplitSbCopySubstrAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "CCCCcommodo consequat. ");
    ins.insertPointer(ins.end(), "Duis aute irure dolor inD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin() + 4, ins.end() - 1));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertDoubleSplitSbCopySubstrAtSplit_ContentEqualsInsertedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "Ccommodo consequat. ");
    ins.insertPointer(ins.end(), "D");
    ins.insertPointer(ins.end(), "uis aute irure dolor inDDDDD");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.insertPointer(msg.begin(), "ex ea "));
    REQUIRE(&msg == &msg.insertPointer(msg.end(), " reprehenderit"));
    REQUIRE(&msg == &msg.insertCopy(msg.begin() + 6, ins.begin() + 1, ins.end() - 5));
    REQUIRE(msg.toString() == "ex ea commodo consequat. Duis aute irure dolor in reprehenderit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer("dolore eu fugiat nulla pariatur"));
    REQUIRE(msg.toString() == "dolore eu fugiat nulla pariatur");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "dolore eu fugiat nul");
    REQUIRE(&msg == &msg.appendPointer("la pariatur"));
    REQUIRE(msg.toString() == "dolore eu fugiat nulla pariatur");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_AppendPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "dolore eu ");
    msg.insertPointer(msg.end(), "fugiat nul");
    REQUIRE(&msg == &msg.appendPointer("la pariatur"));
    REQUIRE(msg.toString() == "dolore eu fugiat nulla pariatur");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendZeroLengthPointer_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "dolore eu fugiat nulla pariatur");
    REQUIRE(&msg == &msg.appendPointer("AAAA", 0));
    REQUIRE(msg.toString() == "dolore eu fugiat nulla pariatur");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendNullPointer_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "dolore eu fugiat nulla pariatur");
    REQUIRE(&msg == &msg.appendPointer(NULL));
    REQUIRE(&msg == &msg.appendPointer(NULL, 10000));
    REQUIRE(msg.toString() == "dolore eu fugiat nulla pariatur");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy("Excepteur sint occaecat"));
    REQUIRE(msg.toString() == "Excepteur sint occaecat");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendHugeCopy_ContentEqualsInsertedText)
{
    AQLogStringBuilder msg(0);
    ostringstream ss;
    size_t total = 0;
    while (total < 256 * 1024)
    {
        REQUIRE(&msg == &msg.appendCopy(LONG_STR));
        ss << LONG_STR;
        total += LONG_STR_LEN;
    }
    REQUIRE(msg.toString() == ss.str());
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Excepteur sint");
    REQUIRE(&msg == &msg.appendCopy(string(" occaecat")));
    REQUIRE(msg.toString() == "Excepteur sint occaecat");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_AppendCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Excepteur");
    msg.insertPointer(msg.end(), " sint ");
    REQUIRE(&msg == &msg.appendCopy("occaecat"));
    REQUIRE(msg.toString() == "Excepteur sint occaecat");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendZeroLengthCopy_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Excepteur sint occaecat");
    REQUIRE(&msg == &msg.appendCopy("AAAA", 0));
    REQUIRE(msg.toString() == "Excepteur sint occaecat");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendNullCopy_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Excepteur sint occaecat");
    REQUIRE(&msg == &msg.appendCopy(NULL));
    REQUIRE(&msg == &msg.appendCopy(NULL, 10000));
    REQUIRE(msg.toString() == "Excepteur sint occaecat");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendFree_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendFree(strdup("cupidatat non proident")));
    REQUIRE(msg.toString() == "cupidatat non proident");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendFree_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "cupidatat non");
    REQUIRE(&msg == &msg.appendFree(strdup(" proident")));
    REQUIRE(msg.toString() == "cupidatat non proident");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_AppendFree_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "cupidatat");
    msg.insertPointer(msg.end(), " non p");
    REQUIRE(&msg == &msg.appendFree(strdup("roident")));
    REQUIRE(msg.toString() == "cupidatat non proident");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendZeroLengthFree_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "cupidatat non proident");
    char *str = strdup("AAAA");
    REQUIRE(&msg == &msg.appendFree(str, 0));
    free(str);
    REQUIRE(msg.toString() == "cupidatat non proident");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendNullFree_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "cupidatat non proident");
    REQUIRE(&msg == &msg.appendFree(NULL));
    REQUIRE(&msg == &msg.appendFree(NULL, 10000));
    REQUIRE(msg.toString() == "cupidatat non proident");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendEmpty_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    APPEND_EMPTY_WRAPPER("sunt in culpa qui");
    REQUIRE(msg.toString() == "sunt in culpa qui");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendEmpty_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sunt in cu");
    APPEND_EMPTY_WRAPPER("lpa qui");
    REQUIRE(msg.toString() == "sunt in culpa qui");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_AppendEmpty_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sunt in");
    msg.insertPointer(msg.end(), " culpa q");
    APPEND_EMPTY_WRAPPER("ui");
    REQUIRE(msg.toString() == "sunt in culpa qui");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendZeroLengthEmpty_ContentUnchanged)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sunt in culpa qui");
    REQUIRE((msg.appendEmpty(0) == NULL));
    REQUIRE(msg.toString() == "sunt in culpa qui");
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSingleSbPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "officia deserunt mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSingleSbPointerSubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAofficia deserunt mollit anim id est laborumB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer(ins.begin() + 3, ins.end() - 1));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSplitSbPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "officia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSplitSbPointerSubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "JJJJJJofficia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborumKK");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer(ins.begin() + 6, ins.end() - 2));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendDoubleSbPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "officia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim i");
    ins.insertPointer(ins.end(), "d est laborum");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendDoubleSbPointerSubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "Aofficia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim i");
    ins.insertPointer(ins.end(), "d est laborumBBBBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendPointer(ins.begin() + 1, ins.end() - 8));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSingleSbPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "deserunt mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia ");
    REQUIRE(&msg == &msg.appendPointer(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSingleSbPointerSubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "deserunt mollit anim id est laborumXXXXXX");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia ");
    REQUIRE(&msg == &msg.appendPointer(ins.begin(), ins.end() - 6));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSplitSbPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "eserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia d");
    REQUIRE(&msg == &msg.appendPointer(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSplitSbPointerSubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "AAAAAAeserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia d");
    REQUIRE(&msg == &msg.appendPointer(ins.begin() + 6, ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendDoubleSbPointer_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "lit anim i");
    ins.insertPointer(ins.end(), "d est la");
    ins.insertPointer(ins.end(), "borum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia deserunt mol");
    REQUIRE(&msg == &msg.appendPointer(ins));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendDoubleSbPointerSubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "Xlit anim i");
    ins.insertPointer(ins.end(), "d est la");
    ins.insertPointer(ins.end(), "borumY");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia deserunt mol");
    REQUIRE(&msg == &msg.appendPointer(ins.begin() + 1, ins.end() - 1));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSingleSbCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "officia deserunt mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSingleSbCopySubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "AAAofficia deserunt mollit anim id est laborumB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy(ins.begin() + 3, ins.end() - 1));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSplitSbCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "officia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendSplitSbCopySubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "JJJJJJofficia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborumKK");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy(ins.begin() + 6, ins.end() - 2));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendDoubleSbCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "officia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim i");
    ins.insertPointer(ins.end(), "d est laborum");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy(ins));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_AppendDoubleSbCopySubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "Aofficia deserunt ");
    ins.insertPointer(ins.end(), "mollit anim i");
    ins.insertPointer(ins.end(), "d est laborumBBBBBBBB");
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendCopy(ins.begin() + 1, ins.end() - 8));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSingleSbCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "deserunt mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia ");
    REQUIRE(&msg == &msg.appendCopy(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSingleSbCopySubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.begin(), "deserunt mollit anim id est laborumXXXXXX");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia ");
    REQUIRE(&msg == &msg.appendCopy(ins.begin(), ins.end() - 6));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSplitSbCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "eserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia d");
    REQUIRE(&msg == &msg.appendCopy(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendSplitSbCopySubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "AAAAAAeserunt ");
    ins.insertPointer(ins.end(), "mollit anim id est laborum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia d");
    REQUIRE(&msg == &msg.appendCopy(ins.begin() + 6, ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendDoubleSbCopy_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "lit anim i");
    ins.insertPointer(ins.end(), "d est la");
    ins.insertPointer(ins.end(), "borum");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia deserunt mol");
    REQUIRE(&msg == &msg.appendCopy(ins.begin(), ins.end()));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_AppendDoubleSbCopySubstr_ContentEqualsConcatenatedText)
{
    AQLogStringBuilder ins(0);
    ins.insertPointer(ins.end(), "Xlit anim i");
    ins.insertPointer(ins.end(), "d est la");
    ins.insertPointer(ins.end(), "borumY");
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "officia deserunt mol");
    REQUIRE(&msg == &msg.appendCopy(ins.begin() + 1, ins.end() - 1));
    REQUIRE(msg.toString() == "officia deserunt mollit anim id est laborum");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_Appendf_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendf("%s %d %s", "Lorem ipsum dolor sit amet", 666345, ", consectetur adipiscing elit"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet 666345 , consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_Appendf_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Lorem ipsum");
    REQUIRE(&msg == &msg.appendf("%s %d %s", " dolor sit amet", 666345, ", consectetur adipiscing elit"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet 666345 , consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_Appendf_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Lorem ipsum");
    msg.insertPointer(msg.end(), " dolor ");
    REQUIRE(&msg == &msg.appendf("%s %d %s", "sit amet", 666345, ", consectetur adipiscing elit"));
    REQUIRE(msg.toString() == "Lorem ipsum dolor sit amet 666345 , consectetur adipiscing elit");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_Insertf_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(74 == msg.insertf(msg.begin(), "%s %d %s", "sed do eiusmod tempor incididunt", -421122, " ut labore et dolore magna aliqua"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertfAtBegin_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "na aliqua");
    REQUIRE(74 - 9 == msg.insertf(msg.begin(), "%s %d %s", "sed do eiusmod tempor incididunt", -421122, " ut labore et dolore mag"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertfAtMid_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sed do eiue magna aliqua");
    REQUIRE(74 - 10 - 14 == msg.insertf(msg.begin() + 10, "%s %d %s", "smod tempor incididunt", -421122, " ut labore et dolor"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertfAtEnd_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sed do e");
    REQUIRE(74 - 8 == msg.insertf(msg.end(), "%s %d %s", "iusmod tempor incididunt", -421122, " ut labore et dolore magna aliqua"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertfAtSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sed do eiu");
    msg.insertPointer(msg.end(), "e magna aliqua");
    REQUIRE(74 - 10 - 14 == msg.insertf(msg.begin() + 10, "%s %d %s", "smod tempor incididunt", -421122, " ut labore et dolor"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertfBeforeSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sed do eiue");
    msg.insertPointer(msg.end(), " magna aliqua");
    REQUIRE(74 - 10 - 14 == msg.insertf(msg.begin() + 10, "%s %d %s", "smod tempor incididunt", -421122, " ut labore et dolor"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertfAfterSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "sed do ei");
    msg.insertPointer(msg.end(), "ue magna aliqua");
    REQUIRE(74 - 10 - 14 == msg.insertf(msg.begin() + 10, "%s %d %s", "smod tempor incididunt", -421122, " ut labore et dolor"));
    REQUIRE(msg.toString() == "sed do eiusmod tempor incididunt -421122  ut labore et dolore magna aliqua");
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_Appendftime_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(&msg == &msg.appendftime(STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == STRFTIME_TEST_TIME_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_Appendftime_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "Ut enim ad minim veniam");
    REQUIRE(&msg == &msg.appendftime(" " STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "Ut enim ad minim veniam " STRFTIME_TEST_TIME_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_Appendftime_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), " minim veniam");
    msg.insertPointer(msg.begin(), "Ut enim ad");
    REQUIRE(&msg == &msg.appendftime(" " STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "Ut enim ad minim veniam " STRFTIME_TEST_TIME_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_LargeAppendftime_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), " minim veniam");
    msg.insertPointer(msg.begin(), "Ut enim ad");
    REQUIRE(&msg == &msg.appendftime(
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        , STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "Ut enim ad minim veniam"
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        );
}

//------------------------------------------------------------------------------
TEST(given_SbEmpty_when_Inserttimef_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) - 1 == msg.insertftime(msg.begin(), STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == STRFTIME_TEST_TIME_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertftimeAtBegin_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) == msg.insertftime(msg.begin(), STRFTIME_TEST_TIME_FMT " ", STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == STRFTIME_TEST_TIME_STR " quis nostrud exercitation");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertftimeAtMid_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) + 1 == msg.insertftime(msg.begin() + 8, "<" STRFTIME_TEST_TIME_FMT ">", STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "quis nos<" STRFTIME_TEST_TIME_STR ">trud exercitation");
}

//------------------------------------------------------------------------------
TEST(given_SbSingle_when_InsertftimeAtEnd_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitation");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) == msg.insertftime(msg.end() + 8, "|" STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "quis nostrud exercitation|" STRFTIME_TEST_TIME_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertftimeAtSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitatio");
    msg.insertPointer(msg.end(), "n");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) - 1 == msg.insertftime(msg.end() - 1, STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "quis nostrud exercitatio" STRFTIME_TEST_TIME_STR "n");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertftimeBeforeSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "quis nostrud exercitatio");
    msg.insertPointer(msg.end(), "n");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) - 1 == msg.insertftime(msg.end() - 2, STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "quis nostrud exercitati" STRFTIME_TEST_TIME_STR "on");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_InsertftimeAfterSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "q");
    msg.insertPointer(msg.end(), "uis nostrud exercitation");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) - 1 == msg.insertftime(msg.begin() + 2, STRFTIME_TEST_TIME_FMT, STRFTIME_TEST_TIME));
    REQUIRE(msg.toString() == "qu" STRFTIME_TEST_TIME_STR "is nostrud exercitation");
}

//------------------------------------------------------------------------------
TEST(given_SbSplit_when_LargeInsertftimeAfterSplit_ContentEqualsFormattedText)
{
    AQLogStringBuilder msg(0);
    msg.insertPointer(msg.begin(), "q");
    msg.insertPointer(msg.end(), "uis nostrud exercitation");
    REQUIRE(sizeof(STRFTIME_TEST_TIME_STR) * 4 * 16 == msg.insertftime(msg.begin() + 2,
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT ":" STRFTIME_TEST_TIME_FMT
        , STRFTIME_TEST_TIME));

    REQUIRE(msg.toString() == "qu"
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR ":" STRFTIME_TEST_TIME_STR
        "is nostrud exercitation");
}

//------------------------------------------------------------------------------
TEST(given_EmptyCoalesce_when_AppendSequentialBuffers_SingleIovCreated)
{
    AQLogStringBuilder msg;
    msg.appendPointer(&LONG_STR_BUF[0], 12);
    msg.appendPointer(&LONG_STR_BUF[12], 37);
    msg.appendPointer(&LONG_STR_BUF[12 + 37], LONG_STR_LEN - (12 + 37));
    REQUIRE(msg.toString() == LONG_STR);
    REQUIRE(msg.iovCount() == 1);
}

//------------------------------------------------------------------------------
TEST(given_SingleCoalesce_when_AppendSequentialBuffers_SingleIovCreated)
{
    AQLogStringBuilder msg;
    msg.appendCopy(&LONG_STR_BUF[0], 2);
    msg.appendPointer(&LONG_STR_BUF[2], 18);
    msg.appendPointer(&LONG_STR_BUF[2 + 18], 55);
    msg.appendPointer(&LONG_STR_BUF[2 + 18 + 55], LONG_STR_LEN - (2 + 18 + 55));
    REQUIRE(msg.toString() == LONG_STR);
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SplitCoalesce_when_InsertSequentialWithPrevious_SingleIovCreated)
{
    AQLogStringBuilder msg;
    msg.appendPointer(&LONG_STR_BUF[0], 18);
    msg.appendCopy(&LONG_STR_BUF[18 + 77], LONG_STR_LEN - (18 + 77));
    msg.insertPointer(msg.begin() + 18, &LONG_STR_BUF[18], 77);
    REQUIRE(msg.toString() == LONG_STR);
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SplitCoalesce_when_InsertSequentialWithNext_SingleIovCreated)
{
    AQLogStringBuilder msg;
    msg.appendCopy(&LONG_STR_BUF[0], 18);
    msg.appendPointer(&LONG_STR_BUF[18 + 77], LONG_STR_LEN - (18 + 77));
    msg.insertPointer(msg.begin() + 18, &LONG_STR_BUF[18], 77);
    REQUIRE(msg.toString() == LONG_STR);
    REQUIRE(msg.iovCount() == 2);
}

//------------------------------------------------------------------------------
TEST(given_SplitCoalesce_when_PopulateFragmented_BuffersCoalesced)
{
    AQLogStringBuilder msg;
    size_t f = makeFragmentedStringBuilder(msg);
    REQUIRE(msg.iovCount() < f);
    REQUIRE(msg.toString() == FRAG_STR);
}

//------------------------------------------------------------------------------
TEST(given_SbFragmented_when_WrittenToOstream_OstreamContainsString)
{
    AQLogStringBuilder msg;
    makeFragmentedStringBuilder(msg);
    ostringstream ss;
    ss << msg;
    REQUIRE(ss.str() == FRAG_STR);
}




//=============================== End of File ==================================
