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

#include "AQTest.h"
#include "TestPointAction.h"




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
TEST_SUITE(UtClaim);

//------------------------------------------------------------------------------
AQTEST(when_Size0_then_ClaimFails)
{
    AQWriterItem witem;

    REQUIRE_EXCEPTION(aq.writer.claim(witem, 0), invalid_argument);
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(when_SizeLarge_then_ClaimFails)
{
    AQWriterItem witem;
    REQUIRE_EXCEPTION(aq.writer.claim(witem, CtrlOverlay::CTRLQ_SIZE_MASK + 1), invalid_argument);
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(when_Claim1Byte_then_AvailableFallsBy1Page)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, 1));
    REQUIRE(aq.isItemPage(witem, 0, 1));
    REQUIRE(aq.writer.availableSize() == (aq.pageCount() - 2) * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(when_Claim1Page_then_AvailableFallsBy1Page)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize()));
    REQUIRE(aq.writer.availableSize() == (aq.pageCount() - 2) * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(when_Claim1PagePlus1Byte_then_AvailableFallsBy2Pages)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() + 1));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() + 1));
    REQUIRE(aq.writer.availableSize() == (aq.pageCount() - 3) * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(when_Claim2Pages_then_AvailableFallsBy2Pages)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 2));
    REQUIRE(aq.writer.availableSize() == (aq.pageCount() - 3) * aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt0_when_ClaimPageCountTake1_then_ClaimSucceeds)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * (aq.pageCount() - 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * (aq.pageCount() - 1)));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * (aq.pageCount() - 1)));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt0_when_ClaimPageCount_then_ClaimFails)
{
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * (aq.pageCount() - 1) + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * (aq.pageCount() - 1));
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt1_when_ClaimPageCountTake1_then_ClaimSucceeds)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * (aq.pageCount() - 1));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * (aq.pageCount() - 1)));
    REQUIRE(aq.isItemPage(witem, 1, aq.pageSize() * (aq.pageCount() - 1)));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt1_when_ClaimPageCount_then_ClaimFails)
{
    aq.advance(1);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * (aq.pageCount() - 1) + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * (aq.pageCount() - 1));
}
//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt2_when_ClaimPageCountTake2_then_ClaimSucceeds)
{
    aq.advance(2);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * (aq.pageCount() - 2));
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * (aq.pageCount() - 2)));
    REQUIRE(aq.isItemPage(witem, 2, aq.pageSize() * (aq.pageCount() - 2)));
    REQUIRE(aq.writer.availableSize() == aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt2_when_ClaimPageCountTake1_then_ClaimFails)
{
    aq.advance(2);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * (aq.pageCount() - 2) + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * (aq.pageCount() - 2));
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt6_when_Claim5_then_ClaimSucceeds)
{
    aq.advance(6);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 5);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 5));
    REQUIRE(aq.isItemPage(witem, 6, aq.pageSize() * 5));
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 5);
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt6_when_Claim6_then_ClaimFails)
{
    aq.advance(6);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 5 + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 5);
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt7_when_Claim4_then_ClaimSucceeds)
{
    aq.advance(7);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 4));
    REQUIRE(aq.isItemPage(witem, 7, aq.pageSize() * 4));
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt7_when_Claim5_then_ClaimSuccessAt0)
{
    aq.advance(7);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 5));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 5));
    REQUIRE(aq.writer.availableSize() == aq.pageSize());
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt7_when_Claim6_then_ClaimSuccessAt0)
{
    aq.advance(7);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 6));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 6));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt7_when_Claim7_then_ClaimFails)
{
    aq.advance(7);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 6 + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
}

//------------------------------------------------------------------------------
AQTEST(given_Head0Tail8_when_Claim2_then_ClaimSucceeds)
{
    aq.enqueue(8);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    REQUIRE(aq.isItemPage(witem, 8, aq.pageSize() * 2));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Head0Tail8_when_Claim3_then_ClaimFails)
{
    aq.enqueue(8);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
}

//------------------------------------------------------------------------------
AQTEST(given_Head1Tail8_when_Claim3_then_ClaimSucceeds)
{
    aq.advance(1);
    aq.enqueue(7);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 3);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(aq.isItemPage(witem, 8, aq.pageSize() * 3));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Head1Tail8_when_Claim4_then_ClaimFails)
{
    aq.advance(1);
    aq.enqueue(7);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 4));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 3);
}

//------------------------------------------------------------------------------
AQTEST(given_Head8Tail0_when_Claim7_then_ClaimSucceeds)
{
    aq.advance(8);
    aq.enqueue(3);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 7);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 7));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 7));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Head8Tail0_when_Claim8_then_ClaimFails)
{
    aq.advance(8);
    aq.enqueue(3);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 7 + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 7);
}

//------------------------------------------------------------------------------
AQTEST(given_Head8Tail1_when_Claim6_then_ClaimSucceeds)
{
    aq.advance(8);
    aq.enqueue(4);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 6));
    REQUIRE(aq.isItemPage(witem, 1, aq.pageSize() * 6));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Head8Tail1_when_Claim7_then_ClaimFails)
{
    aq.advance(8);
    aq.enqueue(4);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 6 + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 6);
}

//------------------------------------------------------------------------------
AQTEST(given_Head10Tail0_when_Claim9_then_ClaimSucceeds)
{
    aq.advance(10);
    aq.enqueue(1);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 9);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 9));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 9));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Head10Tail0_when_Claim10_then_ClaimFails)
{
    aq.advance(10);
    aq.enqueue(1);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 9 + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 9);
}

//------------------------------------------------------------------------------
AQTEST(given_Head10Tail1_when_Claim8_then_ClaimSucceeds)
{
    aq.advance(10);
    aq.enqueue(2);
    AQWriterItem witem;
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 8);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 8));
    REQUIRE(aq.isItemPage(witem, 1, aq.pageSize() * 8));
    REQUIRE(aq.writer.availableSize() == 0);
}

//------------------------------------------------------------------------------
AQTEST(given_Head10Tail1_when_Claim9_then_ClaimFails)
{
    aq.advance(10);
    aq.enqueue(2);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 8 + 1));
    REQUIRE(!witem.isAllocated());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 8);
}

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt0_when_Claim1PreemptHeadClaim1_then_BothClaimsSucceed)
{
    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize());

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);

    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 8);
    REQUIRE(aq.isItemPage(ca.item(), 0, aq.pageSize()));
    REQUIRE(aq.isItemPage(witem, 1, aq.pageSize()));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt0_when_Claim1PreemptCtrlClaim1_then_BothClaimsSucceed)
{
    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize());

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);

    REQUIRE(aq.writer.claim(witem, aq.pageSize()));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 8);
    REQUIRE(aq.isItemPage(ca.item(), 1, aq.pageSize()));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize()));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim3PreemptHeadClaim4_then_BothClaimsSucceed)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 4);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == 0);
    REQUIRE(aq.isItemPage(ca.item(), 0, aq.pageSize() * 4));
    REQUIRE(aq.isItemPage(witem, 4, aq.pageSize() * 3));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim3PreemptCtrlClaim4_then_BothClaimsSucceed)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 4);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 3);
    REQUIRE(aq.isItemPage(ca.item(), 0, aq.pageSize() * 4));
    REQUIRE(aq.isItemPage(witem, 8, aq.pageSize() * 3));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim3PreemptSkipCtrlClaim4_then_BothClaimsSucceed)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 4);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 3);
    REQUIRE(aq.isItemPage(ca.item(), 0, aq.pageSize() * 4));
    REQUIRE(aq.isItemPage(witem, 8, aq.pageSize() * 3));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim5PreemptHeadClaim3_then_BothClaimsSucceed)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 3);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 5));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
    REQUIRE(aq.isItemPage(ca.item(), 8, aq.pageSize() * 3));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 5));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim5PreemptCtrlClaim3_then_Claim5Fail)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 3);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 5));
    REQUIRE(!ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
    REQUIRE(!ca.item().isAllocated());
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 5));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim5PreemptSkipCtrlClaim3_then_Claim5Fail)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 3);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrlSkipPages);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 5));
    REQUIRE(!ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
    REQUIRE(!ca.item().isAllocated());
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 5));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim3PreemptHeadClaim5_then_Claim5Fail)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 5);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);

    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
    REQUIRE(aq.isItemPage(ca.item(), 0, aq.pageSize() * 5));
    REQUIRE(!witem.isAllocated());
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim3PreemptCtrlClaim5_then_BothClaimsSucceed)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize() * 5);

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3));
    REQUIRE(ca.result());
    REQUIRE(aq.writer.availableSize() == aq.pageSize() * 2);
    REQUIRE(aq.isItemPage(ca.item(), 0, aq.pageSize() * 5));
    REQUIRE(aq.isItemPage(witem, 8, aq.pageSize() * 3));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim2PreemptHeadClaim5PreemptHeadClaim3_then_AllClaimsSucceed)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca3(aq.writer, aq.pageSize() * 3);
    ClaimAction ca5(aq.writer, aq.pageSize() * 5);

    ca5.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);
    ca5.chainBefore(aq.writer, AQWriter::ClaimBeforeWriteHeadRef, ca3);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    REQUIRE(ca5.result());
    REQUIRE(ca3.result());
    REQUIRE(aq.writer.availableSize() == 0);
    REQUIRE(aq.isItemPage(ca5.item(), 0, aq.pageSize() * 5));
    REQUIRE(aq.isItemPage(ca3.item(), 8, aq.pageSize() * 3));
    REQUIRE(aq.isItemPage(witem, 5, aq.pageSize() * 2));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_HeadTailAt8_when_Claim2PreemptHeadClaim5PreemptCtrlClaim3_then_Claim3Fails)
{
    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca3(aq.writer, aq.pageSize() * 3);
    ClaimAction ca5(aq.writer, aq.pageSize() * 5);

    ca5.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);
    ca5.chainBefore(aq.writer, AQWriter::ClaimBeforeWriteCtrl, ca3);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));
    REQUIRE(ca5.result());
    REQUIRE(!ca3.result());
    REQUIRE(aq.writer.availableSize() == 0);
    REQUIRE(aq.isItemPage(ca5.item(), 0, aq.pageSize() * 5));
    REQUIRE(!ca3.item().isAllocated());
    REQUIRE(aq.isItemPage(witem, 5, aq.pageSize() * 2));
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_Claim_when_PreemptHeadWriteWithAnotherClaim_then_ContentionCountIncreasesBy1)
{
    REQUIRE(aq.reader.claimContentionCount() == 0);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize());

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);

    REQUIRE(aq.writer.claim(witem, aq.pageSize()));

    REQUIRE(aq.reader.claimContentionCount() == 1);
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_Claim_when_PreemptCtrlWriteWithAnotherClaim_then_ContentionCountUnchanged)
{
    REQUIRE(aq.reader.claimContentionCount() == 0);

    AQWriterItem witem;

    ClaimAction ca(aq.writer, aq.pageSize());

    ca.attach(aq.writer, AQWriter::ClaimBeforeWriteCtrl);

    REQUIRE(aq.writer.claim(witem, aq.pageSize()));

    REQUIRE(aq.reader.claimContentionCount() == 0);
}
#endif

//------------------------------------------------------------------------------
#ifdef AQ_TEST_POINT
AQTEST(given_Claim_when_PreemptHeadWriteWithAnotherClaimTwice_then_ContentionCountIncreasesBy2)
{
    REQUIRE(aq.reader.claimContentionCount() == 0);

    aq.advance(8);

    AQWriterItem witem;

    ClaimAction ca3(aq.writer, aq.pageSize() * 3);
    ClaimAction ca5(aq.writer, aq.pageSize() * 5);

    ca5.attach(aq.writer, AQWriter::ClaimBeforeWriteHeadRef);
    ca5.chainBefore(aq.writer, AQWriter::ClaimBeforeWriteHeadRef, ca3);

    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2));

    REQUIRE(aq.reader.claimContentionCount() == 2);
}
#endif

//------------------------------------------------------------------------------
AQTEST(given_EmptyQueue_when_ClaimEqual11Pages_then_ClaimFails)
{
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 11, AQWriter::CLAIM_EXACT));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_EmptyQueue_when_ClaimEqual10Pages1Byte_then_ClaimFails)
{
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, 1 + aq.pageSize() * 10, AQWriter::CLAIM_EXACT));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_EmptyQueue_when_ClaimAtMost10Pages1Byte_then_Claim10Pages)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, 1 + aq.pageSize() * 10, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 10));
}

//------------------------------------------------------------------------------
AQTEST(given_EmptyQueue_when_ClaimAtMost11Pages_then_Claim10Pages)
{
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 11, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 10));
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt9_when_ClaimEqual3Pages_then_ClaimSucceedsAtStart)
{
    aq.advance(9);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3, AQWriter::CLAIM_EXACT));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 3));
}

//------------------------------------------------------------------------------
AQTEST(given_HeadTailAt9_when_ClaimAtMost3Pages_then_ClaimSucceedsAtStart)
{
    aq.advance(9);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 3));
}

//------------------------------------------------------------------------------
AQTEST(given_Head4TailAt9_when_ClaimEquals4Pages1Byte_then_ClaimFails)
{
    aq.advance(9);
    aq.enqueue(6);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 4 + 1, AQWriter::CLAIM_EXACT));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_Head4TailAt9_when_ClaimEquals5Pages_then_ClaimFails)
{
    aq.advance(9);
    aq.enqueue(6);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 5, AQWriter::CLAIM_EXACT));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_Head4TailAt9_when_ClaimAtMost4Pages1Byte_then_Claim4Pages)
{
    aq.advance(9);
    aq.enqueue(6);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 4 + 1, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 4, aq.pageSize() * 4));
}

//------------------------------------------------------------------------------
AQTEST(given_Head4TailAt9_when_ClaimAtMost5Pages_then_Claim4Pages)
{
    aq.advance(9);
    aq.enqueue(6);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 5, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 4, aq.pageSize() * 4));
}

//------------------------------------------------------------------------------
AQTEST(given_Head9TailAt3_when_ClaimEquals3Pages_then_ClaimFails)
{
    aq.advance(3);
    aq.enqueue(7);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 3, AQWriter::CLAIM_EXACT));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_Head9TailAt3_when_ClaimAtMost3Pages_then_Claim1Pages)
{
    aq.advance(3);
    aq.enqueue(7);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 10, aq.pageSize() * 1));
}

//------------------------------------------------------------------------------
AQTEST(given_Head9TailAt3_when_ClaimAtMost2Pages_then_Claim2PagesAtStart)
{
    aq.advance(3);
    aq.enqueue(7);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 2, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 2));
}

//------------------------------------------------------------------------------
AQTEST(given_Head10TailAt3_when_ClaimEquals3Pages_then_ClaimFails)
{
    aq.advance(3);
    aq.enqueue(8);
    AQWriterItem witem;
    REQUIRE(!aq.writer.claim(witem, aq.pageSize() * 3, AQWriter::CLAIM_EXACT));
    REQUIRE(!witem.isAllocated());
}

//------------------------------------------------------------------------------
AQTEST(given_Head10TailAt3_when_ClaimAtMost3Pages_then_Claim2Pages)
{
    aq.advance(3);
    aq.enqueue(8);
    AQWriterItem witem;
    REQUIRE(aq.writer.claim(witem, aq.pageSize() * 3, AQWriter::CLAIM_AT_MOST));
    REQUIRE(aq.isItemPage(witem, 0, aq.pageSize() * 2));
}




//=============================== End of File ==================================
