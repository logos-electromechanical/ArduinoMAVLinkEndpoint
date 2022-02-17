#include <gtest/gtest.h>
#include "MAVLinkEndpoint.h"
#include "MockPrint.h"
#include "MockStream.h"

using namespace testing;

class MAVLinkEndpointConstructorTests : public ::testing::Test
{
public:
    void SetUp() { 
        writer_call_count = 0; 
        p = new MockPrint();  
    }

    void TearDown() { delete(p); }

    static size_t test_writer(uint8_t *data, size_t len)
    {
        writer_call_count++;
        return p->write(data, len);
    }
    
    static int              writer_call_count;
    static MockPrint       *p;
    MockStream              s;
    MockPrint               d;
};

int MAVLinkEndpointConstructorTests::writer_call_count = 0;

TEST(MAVLinkEndpointConstructorTests, TwoArgs) {
    for (int i = -300; i < 300; i++) 
    {
        for (int j = -300; j < 300; j++) 
        {
            MAVLinkEndpoint mav(i, j);
            if ((i >= 0) && (i <= 255)) EXPECT_EQ(mav.getComponentID(), i); else EXPECT_NE(mav.getSystemID(), i);
            if ((j >= 0) && (j <= 255)) EXPECT_EQ(mav.getSystemID(), j); else EXPECT_NE(mav.getComponentID(), j);
            EXPECT_FALSE(mav.poll());
            EXPECT_FALSE(mav.registerRXCallback(nullptr));
            EXPECT_FALSE(mav.registerRXDefaultCallback(nullptr));
            EXPECT_FALSE(mav.registerTXCallback(nullptr));
            EXPECT_FALSE(mav.registerRXStream(nullptr));
            EXPECT_FALSE(mav.registerTXPrint(nullptr));
            EXPECT_FALSE(mav.registerDebugPrint(nullptr));
            EXPECT_FALSE(mav.process(nullptr, 100));
            EXPECT_NO_THROW(mav.process(nullptr, 100));
            EXPECT_EQ(nullptr, mav.getDebugPrint());
            EXPECT_EQ(nullptr, mav.getTXPrint());
            EXPECT_EQ(nullptr, mav.getRXStream());
            EXPECT_EQ(0, mav.transmit(nullptr));
            mavlink_message_t testmsg;
            EXPECT_EQ(0, mav.transmit(&testmsg));
        }
    }
    EXPECT_EQ(MAVLinkEndpointConstructorTests::writer_call_count, 0);
}