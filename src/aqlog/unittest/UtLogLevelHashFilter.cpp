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

#include "UtLogLevelHash.h"




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtLogLevelHashFilter);

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_InfoMessageSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO);
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_NoticeMessageSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    RandomHandlers rh(hash);
    TestHandler h1(AQLOG_LEVEL_INFO);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoFilter_when_DetailMessageSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO);
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_NoticeAndTwoInfoFilters_when_InfoMessageSubmitted_then_InfoHandlersCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_NOTICE);
    TestHandler h2(AQLOG_LEVEL_INFO);
    TestHandler h3(AQLOG_LEVEL_INFO);
    RandomHandlers rh(hash);
    hash.addHandler(&h2);
    hash.addHandler(&h1);
    hash.addHandler(&h3);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
    REQUIRE(h2.record(0) == &rec);
    REQUIRE(h2.record(1) == NULL);
    REQUIRE(h3.record(0) == &rec);
    REQUIRE(h3.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_NoticeAndTwoInfoFilters_when_NoticeMessageSubmitted_then_AllHandlersCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_NOTICE);
    TestHandler h2(AQLOG_LEVEL_INFO);
    TestHandler h3(AQLOG_LEVEL_INFO);
    hash.addHandler(&h2);
    hash.addHandler(&h1);
    hash.addHandler(&h3);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
    REQUIRE(h2.record(0) == &rec);
    REQUIRE(h2.record(1) == NULL);
    REQUIRE(h3.record(0) == &rec);
    REQUIRE(h3.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoAndTwoNoticeFilters_when_InfoMessageSubmitted_then_InfoHandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO);
    TestHandler h2(AQLOG_LEVEL_NOTICE);
    TestHandler h3(AQLOG_LEVEL_NOTICE);
    RandomHandlers rh(hash);
    hash.addHandler(&h2);
    hash.addHandler(&h1);
    hash.addHandler(&h3);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
    REQUIRE(h2.record(0) == NULL);
    REQUIRE(h3.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoComponentFilter_when_MatchingInfoComponentSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoComponentFilter_when_MatchingNoticeComponentSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoComponentFilter_when_MatchingDetailComponentSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentFilter_when_DifferentComponentSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoTagFilter_when_MatchingInfoTagSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoTagFilter_when_MatchingNoticeTagSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoTagFilter_when_MatchingDetailTagSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_TagFilter_when_DifferentTagSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_foo");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoFileFilter_when_MatchingInfoFileSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "", "file_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoFileFilter_when_MatchingNoticeFileSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "", "file_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_NOTICE, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_InfoFileFilter_when_MatchingDetailFileSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "", "file_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_FileFilter_when_DifferentFileSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "", "file_foo");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFilter_when_MatchingRecordSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFilter_when_MismatchComponentSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo", "tag_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFilter_when_MismatchTagSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "tag_foo");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentFileFilter_when_MatchingRecordSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "", "file_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentFileFilter_when_MismatchComponentSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo", "", "file_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentFileFilter_when_MismatchFileSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "", "file_foo");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_TagFileFilter_when_MatchingRecordSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_bar", "file_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_TagFileFilter_when_MismatchTagSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_foo", "file_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_TagFileFilter_when_MismatchFileSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "", "tag_bar", "file_foo");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFileFilter_when_MatchingRecordSubmitted_then_HandlerCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFileFilter_when_MismatchComponentSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_foo", "tag_bar", "file_bar");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFileFilter_when_MismatchTagSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "tag_foo", "file_bar");
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_ComponentTagFileFilter_when_MismatchFileSubmitted_then_HandlerNotCalled)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_foo");
    hash.addHandler(&h1);
    RandomHandlers rh(hash);

    AQLogRecord rec(AQLOG_LEVEL_INFO, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == NULL);
}

//------------------------------------------------------------------------------
TEST(given_HandlerHasMultipleFilters_when_MatchesAllFilters_then_HandlerCalledOnce)
{
    LogLevelHash hash;
    TestHandler h1(AQLOG_LEVEL_INFO, "comp_bar");
    h1.addFilter(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar");
    h1.addFilter(AQLOG_LEVEL_NOTICE);
    RandomHandlers rh(hash);
    hash.addHandler(&h1);

    AQLogRecord rec(AQLOG_LEVEL_DETAIL, "comp_bar", "tag_bar", "file_bar", "bar");
    hash.handle(rec);

    REQUIRE(h1.record(0) == &rec);
    REQUIRE(h1.record(1) == NULL);
}




//=============================== End of File ==================================
